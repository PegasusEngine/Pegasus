/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BsVm.cpp
//! \author Kleber Garcia
//! \date   20th September 2014
//! \brief  Pegasus blockscript virtual machine, runs a canonical pegasus tree

#include "Pegasus/BlockScript/StackFrameInfo.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/BlockScript/ExpressionEngine.h"
#include "Pegasus/Math/Vector.h"

#define BS_VM_PAGE_SIZE 512

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Canon;

#define SENTINEL 3939

struct FrameInformation
{
    int mPreviousSbp;
    int mIp; //current ip saved
    int mB; //current b saved
    int mSentinel;
};

//******************************************************//
// **************     the commands      ****************//
//******************************************************//


int GetIddOffset(Ast::Idd* idd, BsVmState& state)
{
    if (idd->IsGlobal())
    {
        return state.GetReg(R_G) + idd->GetOffset();
    }
    else
    {
        int frames = idd->GetFrameOffset();
        int sbp = state.GetReg(R_SBP);
        while (frames-- > 0)
        {
            FrameInformation * fi = reinterpret_cast<FrameInformation*>(state.Ram() + sbp - sizeof(FrameInformation));
            PG_ASSERTSTR(fi->mSentinel == SENTINEL,"Memory corruption in stack!!");

            sbp = fi->mPreviousSbp;
        }
        return sbp + idd->GetOffset();
    }
}

int GetMemoryOffset(Ast::Exp* mem, BsVmState& state)
{
    int offset = 0;
    if (mem->GetExpType() == Ast::Idd::sType)
    {
        offset = GetIddOffset(static_cast<Ast::Idd*>(mem), state);
    }
    else
    {
        PG_ASSERT(
            mem->GetExpType() == Ast::Binop::sType &&
            static_cast<Ast::Binop*>(mem)->GetOp() == O_ACCESS &&
            static_cast<Ast::Binop*>(mem)->GetRhs()->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT &&
            static_cast<Ast::Binop*>(mem)->GetLhs()->GetExpType() == Ast::Idd::sType
        );

        Ast::Binop* binop = static_cast<Ast::Binop*>(mem);
        offset = gIntExpEngine.Eval(binop->GetRhs(), state);
        offset = offset + GetIddOffset(static_cast<Ast::Idd*>(binop->GetLhs()), state);
    }
    return offset;
}

int* GetIddMem(Ast::Idd* idd, BsVmState& state)
{
    return reinterpret_cast<int*>(state.Ram() + GetIddOffset(idd, state));
}

void SaveExpression(void* location, Ast::Exp* exp, BsVmState& state)
{
    const TypeDesc* expType = exp->GetTypeDesc();
    int sz = expType->GetByteSize();
    if (expType->GetModifier() == TypeDesc::M_SCALAR)
    {
        int* mem = static_cast<int*>(location);
        switch(expType->GetAluEngine())
        {
        case TypeDesc::E_INT:
            *mem = gIntExpEngine.Eval(exp, state);
            break;
        case TypeDesc::E_FLOAT:
            *mem = reinterpret_cast<int&>(gFloatExpEngine.Eval(exp, state));
            break;
        default:
            PG_FAILSTR("unknown ALU engine for expression.");
        }
    }
    else if (expType->GetModifier() == TypeDesc::M_VECTOR)
    {
        switch(expType->GetAluEngine())
        {
        case TypeDesc::E_FLOAT4:
            *reinterpret_cast<Math::Vec4*>(location) = gFloat4ExpEngine.Eval(exp, state);
            break;
        case TypeDesc::E_FLOAT3:
            *reinterpret_cast<Math::Vec3*>(location) = gFloat3ExpEngine.Eval(exp, state);
            break;
        case TypeDesc::E_FLOAT2:
            *reinterpret_cast<Math::Vec2*>(location) = gFloat2ExpEngine.Eval(exp, state);
            break;
        default:
            PG_FAILSTR("unknown ALU engine for expression.");
        }
    }
    else if (expType->GetModifier() == TypeDesc::M_STRUCT || expType->GetModifier() == TypeDesc::M_ARRAY)
    {
        int*  target = nullptr;
        if (exp->GetExpType() == Ast::Idd::sType)
        {
            Ast::Idd* rhs = static_cast<Ast::Idd*>(exp);
            target = GetIddMem(rhs, state);
            
        }
        else
        {
            PG_ASSERT(
                exp->GetExpType() == Ast::Binop::sType &&
                static_cast<Ast::Binop*>(exp)->GetLhs()->GetExpType() == Ast::Idd::sType &&
                static_cast<Ast::Binop*>(exp)->GetLhs()->GetTypeDesc()->GetModifier() == TypeDesc::M_ARRAY &&
                static_cast<Ast::Binop*>(exp)->GetRhs()->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT
            );

            Ast::Binop* rhs = static_cast<Ast::Binop*>(exp);
            Ast::Idd* arrayIdd = static_cast<Ast::Idd*>(rhs->GetLhs());
            int offset = gIntExpEngine.Eval(rhs->GetRhs(), state);
            target = reinterpret_cast<int*>(reinterpret_cast<char*>(GetIddMem(arrayIdd, state)) + offset);
        }
        Pegasus::Utils::Memcpy(location, target, exp->GetTypeDesc()->GetByteSize());
    }
    else if (expType->GetModifier() == TypeDesc::M_REFERECE || expType->GetModifier() == TypeDesc::M_ENUM || expType->GetModifier() == TypeDesc::M_STAR)
    {
        int val = gIntExpEngine.Eval(exp, state);
        *(reinterpret_cast<int*>(location)) = val;
    }
    else
    {
        PG_FAILSTR("Unhandled save expression command for expression!.");
    }
}

int EvalJmpCond(Ast::Exp* exp, BsVmState& state)
{
    switch (exp->GetTypeDesc()->GetAluEngine())
    {
    case TypeDesc::E_INT:
        {
            int v = gIntExpEngine.Eval(exp, state);
            return v;
        }
        break;
    case TypeDesc::E_FLOAT:
        {
            float f = gFloatExpEngine.Eval(exp, state);
            return f != 0.0 ? 1 : 0;
        }
    }
    return 0;
}

void MoveCommand(Ast::Idd* idd, Ast::Exp* exp, BsVmState& state)
{

    int* dest = GetIddMem(idd, state);
    int byteSize = idd->GetTypeDesc()->GetByteSize();

    if (exp->GetExpType() == Ast::Idd::sType)
    {
        int* target = GetIddMem(static_cast<Ast::Idd*>(exp), state);
        if (byteSize > sizeof(int))
        {
            Pegasus::Utils::Memcpy(dest, target, byteSize);
        }
        else
        {
            *dest = *target;
        }
    }
    else if (exp->GetExpType() == Ast::Imm::sType)
    {
        *dest = static_cast<Ast::Imm*>(exp)->GetVariant().i[0];
    }
    else
    {
        PG_ASSERT(exp->GetExpType() == Ast::Binop::sType || exp->GetExpType() == Ast::Unop::sType);
        SaveExpression(dest, exp, state);
    }
}

void GetObjectPropertyPtrs(BsVmState& state, Ast::Exp* loc, Ast::Exp* obj, const PropertyNode* prop, void** locPtr, void** propertyPtr, int* byteSize)
{
    //get the target memory pointer
    int targetOffset = GetMemoryOffset(loc, state);
    *locPtr = state.Ram() + targetOffset;

    //get the object handle
    int objHandleOffset = GetMemoryOffset(obj, state);
    int objHandle = *reinterpret_cast<int*>(state.Ram() + objHandleOffset);
    GetObjectPropertyRuntimePtrCallback cb = obj->GetTypeDesc()->GetPropertyCallback();
    PG_ASSERTSTR(cb != nullptr, "The property callback cannot be null for this type %s.");
    *propertyPtr = cb(&state, objHandle, prop);

    //get the byte size
    *byteSize = prop->mType->GetByteSize();

}

void ReadObjPropCmd(Canon::ReadObjProp* cmd, BsVmState& state)
{
    const PropertyNode* propertyNode = cmd->GetProp();
    void* locPtr = nullptr;
    void* propertyPtr = nullptr;
    int byteSize = 0;
    GetObjectPropertyPtrs(state, cmd->GetLoc(), cmd->GetObj(), cmd->GetProp(), &locPtr, &propertyPtr, &byteSize);
    if (propertyPtr == nullptr)
    {
        PG_LOG('ERR_', "[BLOCKSCRIPT VIRUAL MACHINE ERROR]: No property %s exists for such object.", propertyNode->mName);
        Utils::Memset8(locPtr, 0, byteSize);
    }
    else
    {
        Utils::Memcpy(locPtr, propertyPtr, byteSize);
    }
}

void WriteObjPropCmd(Canon::WriteObjProp* cmd, BsVmState& state)
{
    const PropertyNode* propertyNode = cmd->GetProp();
    void* locPtr = nullptr;
    void* propertyPtr = nullptr;
    int byteSize = 0;
    GetObjectPropertyPtrs(state, cmd->GetLoc(), cmd->GetObj(), cmd->GetProp(), &locPtr, &propertyPtr, &byteSize);
    if (propertyPtr == nullptr)
    {
        PG_LOG('ERR_', "[BLOCKSCRIPT VIRUAL MACHINE ERROR]: No property %s exists for such object.", propertyNode->mName);
    }
    else
    {
        Utils::Memcpy(propertyPtr, locPtr, byteSize);
    }
}
void SavCommand(Canon::Register r, Ast::Idd* location, BsVmState& state)
{
    int* target = GetIddMem(location, state);
    *target = state.GetReg(r);
}

void LoadCommand(Ast::Exp* tree, Canon::Register r, BsVmState& state)
{
    PG_ASSERT(r < Canon::R_COUNT);
    PG_ASSERT(tree->GetTypeDesc()->GetByteSize() <= CANON_REGISTER_BYTESIZE); // only allowed to fit on registers
    SaveExpression((state.GetRegBuffer() + (int)r), tree, state);
}


void PopFrameCommand(BsVmState& state)
{
    int currFrameSize = state.GetReg(R_ESP) - state.GetReg(R_SBP);
    FrameInformation* currFrame = reinterpret_cast<FrameInformation*>(state.Ram() + state.GetReg(R_SBP) - sizeof(FrameInformation));
    state.SetReg(R_SBP,currFrame->mPreviousSbp);
    state.SetReg(R_ESP, state.GetReg(R_ESP) - (currFrameSize + sizeof(FrameInformation))); 
    state.Shrink(currFrameSize + sizeof(FrameInformation));
    state.DecStackLevels();
}


void PushFrameCommand(const StackFrameInfo* info, BsVmState& state)
{
    if (state.GetStackLevels() >= 0)
    {
        FrameInformation fi = { state.GetReg(R_SBP), state.GetReg(R_IP), state.GetReg(R_B),  SENTINEL};
        state.Grow(info->GetTotalFrameSize() + sizeof(FrameInformation));        
        FrameInformation* currFrameInfo = reinterpret_cast<FrameInformation*>(state.Ram() + state.GetReg(R_ESP));
        *currFrameInfo = fi;
        state.SetReg(R_ESP, state.GetReg(R_ESP) + sizeof(FrameInformation));
        state.SetReg(R_SBP, state.GetReg(R_ESP));
        state.SetReg(R_ESP, state.GetReg(R_ESP) + info->GetTotalFrameSize());
    }
    else
    {
        state.Grow(info->GetTotalFrameSize());  
        state.SetReg(R_ESP, info->GetTotalFrameSize());
        state.SetReg(R_G, state.GetReg(R_SBP));
    }

    state.IncStackLevels();
   
}

void FunRetCommand(BsVmState& state)
{
    FrameInformation* currFrame = reinterpret_cast<FrameInformation*>(state.Ram() + state.GetReg(R_SBP) - sizeof(FrameInformation));
    state.SetReg(R_IP, currFrame->mIp + 1);
    state.SetReg(R_B, currFrame->mB);
    PG_ASSERT(currFrame->mSentinel == SENTINEL);
    PopFrameCommand(state);
}

void FunGoCommand(Canon::FunGo* fungo, BsVmState& state)
{
    Ast::FunCall* fc = fungo->GetFunCall(); 
    const FunDesc* funDesc = fc->GetDesc();
    const Ast::StmtFunDec* funDec = funDesc->GetDec();

    //all expressions run relative to the callers stack, so lets save this stack pointer
    int expressionStack = state.GetReg(R_SBP);
    
    PushFrameCommand(funDec->GetFrame(), state);
    

    //for every type execute and copy to the target scope
    Ast::ExpList * tail = fc->GetArgs();
    int functionStack = state.GetReg(R_SBP);
    int byteOffset = functionStack;
    state.SetReg(R_SBP, expressionStack);
    
    
    while (tail != nullptr && tail->GetExp() != nullptr)
    {
        void* loc = (state.Ram() + byteOffset);
        SaveExpression(loc, tail->GetExp(), state);

        byteOffset += tail->GetExp()->GetTypeDesc()->GetByteSize();
        tail = tail->GetTail();
    }
    
    state.SetReg(R_SBP, functionStack);
    if (funDesc->IsCallback())
    {
        int outputBufferSize = fc->GetTypeDesc()->GetByteSize();
        void* outputBuffer = outputBufferSize > CANON_REGISTER_BYTESIZE
                ? static_cast<void*>(state.Ram() + state.GetReg(R_RET))
                : static_cast<void*>(state.GetRegBuffer() + R_RET) ;
                 
        FunCallbackContext ctx(
            &state,
            funDesc,
            fc->GetArgs(),
            state.Ram() + functionStack,
            byteOffset - functionStack,
            outputBuffer,
            outputBufferSize
        );
        funDesc->GetCallback()(ctx);
        FunRetCommand(state);
    }
    else
    {
        state.SetReg(R_IP, 0);
        state.SetReg(R_B, fungo->GetLabel());
    }
    
}

void IsdhCommmand(Ast::Idd* idd, void* Pointer, BsVmState& state)
{
    int i = state.PushHeapElement(Pointer, idd->GetTypeDesc());
    *GetIddMem(idd, state) = i;
}

void LadrCommand(Canon::Register r, Ast::Exp* mem, BsVmState& state)
{
    int offset = GetMemoryOffset(mem, state);
    state.SetReg(r, offset);
}

void SavdrCommand(Canon::Register lhs, Canon::Register rhs, BsVmState& state)
{
    //get address location from first register
    int* location = reinterpret_cast<int*>(state.Ram() + state.GetReg(lhs));
    *location = state.GetReg(rhs);
}

void CopyToAddrCmd(Register r, Ast::Exp* exp, int byteSize, BsVmState& state)
{
    int* location = reinterpret_cast<int*>(state.Ram() + state.GetReg(r));
    SaveExpression(location, exp, state);
}

void CastCmd(Canon::Cast* cast, BsVmState& state)
{
    union Varying
    {
        int i;
        float f;
    } v;
    
    v.i = state.GetReg(cast->GetRegister());
    Varying result;

    if (cast->IsIntToFloat())
    { 
        result.f = static_cast<float>(v.i);
    }
    else
    {
        result.i = static_cast<int>(v.f);
    }
    state.SetReg(cast->GetRegister(), result.i);
}

BsVmState::BsVmState()
:
    mRam(nullptr),
    mRamSize(0),
    mRamCount(0),
    mAllocator(nullptr),
    mStackLevels(-1),
    mUserContext(nullptr)
{
    Reset();
}

void BsVmState::Initialize(Alloc::IAllocator* allocator)
{
    mAllocator = allocator;
    mHeapContainer.Initialize(allocator);
    Grow(BS_VM_PAGE_SIZE); // try to grow 512 bytes initially
    mRamSize = 0; //reset ram, and keep the page open.
    mStackLevels = -1; //-1 means no stack has been set
}

void BsVmState::Reset()
{
    mRamSize = 0;
    mStackLevels = -1; //-1 means no stack has been set
    for (int i = 0; i < static_cast<int>(Canon::R_COUNT); ++i)
    {
        mR[i] = 0;
    }
    mHeapContainer.Reset();
}

void BsVmState::Grow(int byteCount)
{
    int newRamSize = mRamSize + byteCount;
    if (newRamSize >= mRamCount)
    {
        char* oldRam = mRam;
        int newCount = mRamSize + (1 + (byteCount / BS_VM_PAGE_SIZE)) * BS_VM_PAGE_SIZE;
        mRam = PG_NEW_ARRAY(mAllocator, -1, "BS VM RAM", Alloc::PG_MEM_TEMP, char, newCount);
        if (oldRam != nullptr)
        {
            Utils::Memcpy(mRam, oldRam, mRamCount);
            PG_DELETE_ARRAY(mAllocator, oldRam);
        }
        mRamCount = newCount;
    }
    mRamSize = newRamSize;
}

void BsVmState::Shrink(int byteCount)
{
    mRamSize -= byteCount;
    PG_ASSERT(mRamSize >= 0);
}

BsVmState::~BsVmState()
{
    if (mRam != nullptr)
    {
        PG_DELETE_ARRAY(mAllocator, mRam);
    }
}

void BsVm::Run(const Assembly& assembly, BsVmState& state)
{
    state.Reset();
    while (StepExecution(assembly, state));
}

bool BsVm::StepExecution(const Assembly& assembly, BsVmState& state)
{
    bool active = true;
    const Container<Canon::Block>& blockList = *assembly.mBlocks;
    const Canon::Block* block = &blockList[state.mR[R_B]];
    const Container<Canon::CanonNode*>* nodes = &block->GetStmts();
    int nodeSize = nodes->Size();

    if (state.mR[R_IP] == nodeSize)
    {
        state.mR[R_IP] = 0;
        state.mR[R_B] = block->NextBlock();
        block = &blockList[state.mR[R_B]];
        nodes = &block->GetStmts();
        nodeSize = nodes->Size();
    }
       
    if (nodeSize == 0) 
    {
        //nop! continue execution to next block
        state.mR[R_IP] = 0;
        state.mR[R_B] = block->NextBlock();
        return true;
    }
    Canon::CanonNode* n = (*nodes)[state.mR[R_IP]];
    
    int nodeType = n->GetType();
    
    switch (nodeType)
    {
    case Canon::T_MOVE:
    {
        Canon::Move* mov = static_cast<Canon::Move*>(n);
        MoveCommand(mov->GetLhs(), mov->GetRhs(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_INSERT_DATA_TO_HEAP:
    {
        Canon::InsertDataToHeap* isdh = static_cast<Canon::InsertDataToHeap*>(n);
        IsdhCommmand(isdh->GetTmp(), isdh->GetPointer(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_SAVE:
    {
        Canon::Save* sav = static_cast<Canon::Save*>(n);
        SavCommand(sav->GetRegister(), sav->GetTmp(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_LOAD:
    {
        Canon::Load* load = static_cast<Canon::Load*>(n);
        LoadCommand(load->GetExp(), load->GetRegister(), state);
         ++state.mR[R_IP];
    }
    break;
    case Canon::T_EXIT:
    {
        active = false;
    }
    break;
    case Canon::T_FUNGO:
    {
        Canon::FunGo* fungo = static_cast<Canon::FunGo*>(n);
        FunGoCommand(fungo, state);
    }
    break;
    case Canon::T_JMP:
    {
        state.mR[R_B] = static_cast<Canon::Jmp*>(n)->GetLabel();
        state.mR[R_IP] = 0;
    }
    break;
    case Canon::T_JMPCOND:
    {
        Canon::JmpCond* jmpCond = static_cast<Canon::JmpCond*>(n);
        if (jmpCond->GetComparison() == EvalJmpCond(jmpCond->GetExp(), state))
        {
            state.mR[R_B] = static_cast<Canon::JmpCond*>(n)->GetLabel();
            state.mR[R_IP] = 0;
        }
        else
        {
            ++state.mR[R_IP];
        }
    }
    break;
    case Canon::T_RET:
    {
        FunRetCommand(state);
    }
    break;
    case Canon::T_PUSHFRAME:
    {
        Canon::PushFrame* pushFrame = static_cast<Canon::PushFrame*>(n);
        PushFrameCommand(pushFrame->GetInfo(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_POPFRAME:
    {
        PopFrameCommand(state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_LOAD_ADDR:
    {
        Canon::LoadAddr* ladr = static_cast<Canon::LoadAddr*>(n);
        LadrCommand(ladr->GetRegister(), ladr->GetExp(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_SAVE_TO_ADDR:
    {
        Canon::SaveToAddr* savdr = static_cast<Canon::SaveToAddr*>(n);
        SavdrCommand(savdr->GetLhs(), savdr->GetRhs(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_COPY_TO_ADDR:
    {
        Canon::CopyToAddr* cadr = static_cast<Canon::CopyToAddr*>(n);
        CopyToAddrCmd(cadr->GetRegister(), cadr->GetExp(), cadr->GetByteSize(), state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_CAST:
    {
        Canon::Cast* cast = static_cast<Canon::Cast*>(n);
        CastCmd(cast, state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_READ_OBJ_PROP:
    {
        Canon::ReadObjProp* objProp = static_cast<Canon::ReadObjProp*>(n);
        ReadObjPropCmd(objProp, state);
        ++state.mR[R_IP];
    }
    break;
    case Canon::T_WRITE_OBJ_PROP:
    {
        Canon::WriteObjProp* objProp = static_cast<Canon::WriteObjProp*>(n);
        WriteObjPropCmd(objProp, state);
        ++state.mR[R_IP];
    }
    break;
    default:
        PG_FAILSTR("Unhandled assembly node!");
    }

    return active;
}
