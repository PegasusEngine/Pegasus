/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Canonizer.cpp
//! \author Kleber Garcia
//! \date   15th September 2014
//! \brief  Converts an AST into a canonical tree. Duplicates the nodes that it needs to, leaving
//!         the original AST intact. Implementation

#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Canon;
using namespace Pegasus::BlockScript::Ast;


#define CANON_PAGE_SIZE 256
#define CANON_NEW PG_NEW(&mAllocator, -1, "Canon", Pegasus::Alloc::PG_MEM_TEMP)

void Canonizer::Initialize(Alloc::IAllocator* alloc)
{
    mInternalAllocator = alloc;
    mAllocator.Initialize(CANON_PAGE_SIZE, alloc);
    mBlocks.Initialize(mInternalAllocator);
    mFunBlockMap.Initialize(mInternalAllocator);
    mCurrentBlock = -1;
    mRebuiltExpression = nullptr;
    mRebuiltExpList = nullptr;
    mStrPool.Initialize(alloc);
    mLabelMap.Initialize(alloc);

    mCurrFrames = nullptr;
    mCurrentStackFrame = nullptr;
    mCurrentFunDesc = nullptr;
    mCurrentFunTable = nullptr;
    mCurrentTypeTable = nullptr;
    mCurrentTempAllocationSize = 0;
    mNextLabel = 0;
}


void Canonizer::Reset()
{
    mAllocator.Reset();
    mBlocks.Reset();
    mFunBlockMap.Reset();
    mStrPool.Clear();
    mLabelMap.Reset();
    mCurrentBlock = -1;
    mRebuiltExpression = nullptr;
    mCurrentFunDesc = nullptr;
    mRebuiltExpList = nullptr;
    mCurrFrames = nullptr;
    mCurrentStackFrame = nullptr;
    mCurrentFunTable = nullptr;
    mCurrentTypeTable = nullptr;
    mCurrentTempAllocationSize = 0;
    mNextLabel = 0;
}

int Canonizer::CreateBlock()
{
    Block& b = mBlocks.PushEmpty();
    b.Initialize(mInternalAllocator, mBlocks.Size() - 1);
    return b.GetLabel();
}

void Canonizer::AddBlock(int id)
{
    if (mCurrentBlock != -1)
    {
        Block&  curr = mBlocks[mCurrentBlock];
        curr.SetNextBlock(id);
    }
    mCurrentBlock = id;
}

void Canonizer::PushCanon(CanonNode* n)
{
    Block& currBlock = mBlocks[mCurrentBlock];
    CanonNode*& newCanon = currBlock.GetStmts().PushEmpty();
    newCanon = n;
}

Idd* Canonizer::AllocateTemporal(const TypeDesc* typeDesc)
{
    StackFrameInfo& stackFrameInfo = (*mCurrFrames)[mCurrentStackFrame->GetGuid()];
    int requestSize = typeDesc->GetByteSize();
    if (requestSize + mCurrentTempAllocationSize > stackFrameInfo.GetTempSize())
    {
        stackFrameInfo.AllocateTemporal(requestSize);
    }

    int offset = mCurrentTempAllocationSize;
    mCurrentTempAllocationSize += requestSize;

    char* idd = mStrPool.AllocateString();
    idd[0] = '$';
    idd[1] = 't';
    idd[2] = '\0';
    Idd* iddTree = CANON_NEW Idd(idd);
    iddTree->SetOffset(stackFrameInfo.GetSize() + offset);
    iddTree->SetFrameOffset(0);
    iddTree->SetTypeDesc(typeDesc);

    return iddTree;
}

void Canonizer::ResetTemporals()
{
    mCurrentTempAllocationSize = 0;
}

int Canonizer::GetLabel(const FunDesc* funDesc)
{
    int  labelMapSize = mLabelMap.Size();
    for (int i = 0; i < labelMapSize; ++i)
    {
        Canonizer::FunDescIntPair& p = mLabelMap[i]; 
        if (p.mFunDesc->Equals(funDesc))
        {
            return p.mInt;
        }
    }
    
    return -1;
}

void Canonizer::RegisterFunLabel(const FunDesc* funDesc, int label)
{
    Canonizer::FunDescIntPair& funLabelPair = mLabelMap.PushEmpty();
    funLabelPair.mFunDesc = funDesc;
    funLabelPair.mInt = label;
}

void Canonizer::BuildFunctionAsm()
{
    int funSz = mCurrentFunTable->GetSize();
    for (int i = 0; i < funSz; ++i)
    {
        const FunDesc* fd = mCurrentFunTable->GetDesc(i);

        // if its a callback, ignore and do not generate assembly
        if (fd->IsCallback())
        {
            continue;
        }
        int label = GetLabel(fd);

        if (label == -1)
        {
            label = CreateBlock();
        }

        //record this label with this function
        FunMapEntry& funBlockEntry = mFunBlockMap.PushEmpty();
        funBlockEntry.mFunId = fd->GetGuid();
        funBlockEntry.mAssemblyBlock = label;
       
        AddBlock(label);
        mCurrentBlock = label;
        mCurrentStackFrame = fd->GetDec()->GetFrame();
        mCurrentFunDesc = fd;
        fd->GetDec()->GetStmtList()->Access(this);
        mCurrentFunDesc = nullptr;
        PushCanon( CANON_NEW Ret );
    }
}

void Canonizer::Canonize(
        Program* program,
        TypeTable* typeTable,
        FunTable*  funTable,
        Container<StackFrameInfo>* stackFrameInfos
    )
{
    PG_ASSERTSTR(mBlocks.Size() == 0 && mCurrentBlock == -1, "Must call reset if planning to recanonize!");
    mCurrentTypeTable = typeTable;
    mCurrentFunTable = funTable;
    mCurrFrames = stackFrameInfos;
    program->Access(this);
}

void Canonizer::Visit(Program* n)
{
    AddBlock(CreateBlock());
    mCurrentStackFrame = &(*mCurrFrames)[0];
    PushCanon( CANON_NEW PushFrame(mCurrentStackFrame));
    StmtList* stmtList = n->GetStmtList();
    if (stmtList != nullptr)
    {
        stmtList->Access(this);
    }
    PushCanon( CANON_NEW Exit());
    BuildFunctionAsm();
}

void Canonizer::Visit(Exp* n)
{
    PG_FAILSTR("[Canonizer::Visit(Exp*)] This node should not be visited!");
}

void Canonizer::Visit(ExpList* n)
{
    ExpList* node = n;
    ExpList* newN = CANON_NEW ExpList();
    ExpList* newList = newN;
    
    while (n != nullptr)
    {
        if (n->GetExp() != nullptr)
        {
            n->GetExp()->Access(this);
            newList->SetExp(mRebuiltExpression);
        }

        n = n->GetTail();
        if (n != nullptr)
        {
            newList->SetTail(CANON_NEW ExpList());
            newList = newList->GetTail();
        }
    }
    mRebuiltExpList = newN;
}

void Canonizer::Visit(Stmt* n)
{
    PG_FAILSTR("[Canonizer::Visit(Stmt*)] This node should not be visited!");
}

void Canonizer::Visit(StmtList* n)
{
    StmtList* head = n;
    while (head != nullptr)
    {
        ResetTemporals();
        if (head->GetStmt() != nullptr)
        {
            head->GetStmt()->Access(this);
            ResetTemporals();
        }
        head = head->GetTail();
    }
}

void Canonizer::Visit(ArgDec* n)
{
    PG_FAILSTR("[Canonizer::Visit(Stmt*)] This node should not be visited!");
}

void Canonizer::Visit(ArgList* n)
{
    PG_FAILSTR("[Canonizer::Visit(Stmt*)] This node should not be visited!");
}

void Canonizer::Visit(Idd* n)
{
    // do nothing
    mRebuiltExpression = n;
}

static int GetSwizzleOffset(char swizzleChar)
{
    return (swizzleChar - 'w' + 3) % 4;
}

bool Canonizer::IsContinuousSwizzle(Idd* swizzle) const
{
    const char * name = swizzle->GetName();
    PG_ASSERT(name != nullptr);

    int len = Utils::Strlen(name);
    PG_ASSERT(len <= 4);

    if (len == 1)
    {
        return true;
    }

    for (int i = 1; i < len; ++i)
    {
        PG_ASSERT(name[i] >= 'w' && name[i] <= 'z' && name[i - 1] >= 'w' && name[i - 1] <= 'z');
        int id1 = GetSwizzleOffset(name[i - 1]);
        int id2 = GetSwizzleOffset(name[i]);
        if ((id2 - id1) != 1) return false;
    }

    return true;
}

void Canonizer::HandleArrayAccessOperator(Binop* n)
{
    
    const TypeDesc* arrayType = n->GetTypeDesc();
    n->GetLhs()->Access(this);
    Exp* lhs = mRebuiltExpression;
    Exp* rhs = n->GetRhs();

    PG_ASSERT(n->GetOp() == O_ACCESS);
    PG_ASSERT(rhs->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT);
    
    Variant v;
    v.i[0] = arrayType->GetByteSize();
    Imm* imm = CANON_NEW Imm(v);
    imm->SetTypeDesc(rhs->GetTypeDesc());

    if (lhs->GetExpType() == Binop::sType)
    {
        PG_ASSERT(static_cast<Binop*>(lhs)->GetOp() == O_ACCESS);
        Binop* lhsBinop = static_cast<Binop*>(lhs);
        PG_ASSERT(lhsBinop->GetRhs()->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT);

        Binop* offsetExp = CANON_NEW Binop(imm,O_MUL,rhs);
        offsetExp->SetTypeDesc(rhs->GetTypeDesc());
        
        offsetExp = CANON_NEW Binop(offsetExp, O_PLUS, lhsBinop->GetRhs());
        offsetExp->SetTypeDesc(rhs->GetTypeDesc());

        Binop* finalReference = CANON_NEW Binop(lhsBinop->GetLhs(), O_ACCESS,offsetExp);
        finalReference->SetTypeDesc(arrayType);
        mRebuiltExpression = finalReference;
    }
    else
    {
        PG_ASSERT(lhs->GetExpType() == Idd::sType);
        Exp* offsetExp = CANON_NEW Binop(imm, O_MUL, rhs);
        offsetExp->SetTypeDesc(rhs->GetTypeDesc());
        Binop* finalReference = CANON_NEW Binop(lhs, O_ACCESS, offsetExp);
        finalReference->SetTypeDesc(arrayType);
        mRebuiltExpression = finalReference;
    }
}

void Canonizer::HandleSetOperator(Binop* n)
{
    bool lhsIsNonContinuousSwizzle = 
        (
            n->GetLhs()->GetExpType() == Binop::sType &&
            static_cast<Binop*>(n->GetLhs())->GetOp() == O_DOT &&
            static_cast<Binop*>(n->GetLhs())->GetLhs()->GetTypeDesc()->GetModifier() == TypeDesc::M_VECTOR &&
            static_cast<Binop*>(n->GetLhs())->GetRhs()->GetExpType() == Idd::sType &&
            !IsContinuousSwizzle(static_cast<Idd*>(static_cast<Binop*>(n->GetLhs())->GetRhs()))
        );
    Exp* newLhs = nullptr;
    if (!lhsIsNonContinuousSwizzle)
    {
        n->GetLhs()->Access(this);
        newLhs = mRebuiltExpression;
    }
    else
    {
        newLhs = AllocateTemporal(n->GetLhs()->GetTypeDesc());
    }

    PG_ASSERT(n->GetOp() == O_SET);
    if (n->GetRhs()->GetExpType() == FunCall::sType)
    {   
        FunCall* fc = static_cast<FunCall*>(n->GetRhs());
        //visit all children expression
        fc->GetArgs()->Access(this);
    
        if (fc->GetTypeDesc()->GetByteSize() <= CANON_REGISTER_BYTESIZE)
        {
    
            Idd* retSave = BeginSaveRet();
            ProcessFunCall(fc);
            if (newLhs->GetExpType() == Idd::sType)
            {
                Idd* id = static_cast<Idd*>(newLhs);
                PushCanon( CANON_NEW Save(id, Canon::R_RET));
            }
            else
            {
                PG_ASSERT(newLhs->GetExpType() == Binop::sType && static_cast<Binop*>(newLhs)->GetOp() == O_ACCESS);
                PushCanon( CANON_NEW LoadAddr(Canon::R_C, newLhs));
                PushCanon( CANON_NEW SaveToAddr(Canon::R_C, Canon::R_RET) ); 
            }

            EndSaveRet(retSave);


        }
        else
        {
            Idd* retSave = BeginSaveRet();
            //load the address of the lhs so the function can write into it
            PushCanon( CANON_NEW LoadAddr(Canon::R_RET, newLhs));
            ProcessFunCall(fc);
            EndSaveRet(retSave);
        }
    }
    else if (
        n->GetRhs()->GetExpType() == Binop::sType && 
        static_cast<Binop*>(n->GetRhs())->GetOp() == O_ARRAY_CONSTRUCTOR
    )
    {
        PG_ASSERT(newLhs->GetExpType() == Idd::sType); //dont allow array declarations on existant types
        mRebuiltExpression = newLhs;
    }
    else
    {
        n->GetRhs()->Access(this);
        if (newLhs->GetExpType() == Idd::sType)
        {
            PushCanon( CANON_NEW Move(static_cast<Idd*>(newLhs), mRebuiltExpression));
        }
        else
        {
            PG_ASSERT(newLhs->GetExpType() == Binop::sType && static_cast<Binop*>(newLhs)->GetOp() == O_ACCESS);
            PushCanon( CANON_NEW LoadAddr(Canon::R_C, newLhs));
            PushCanon( CANON_NEW CopyToAddr(Canon::R_C, mRebuiltExpression, newLhs->GetTypeDesc()->GetByteSize()));
        }
    }

    if (lhsIsNonContinuousSwizzle)
    {
        PG_ASSERT(newLhs->GetExpType() == Idd::sType);
        PG_ASSERT(n->GetLhs()->GetExpType() == Binop::sType);
        Idd* resultTemp = static_cast<Idd*>(newLhs);
        Binop* lhsBinop = static_cast<Binop*>(n->GetLhs());
        lhsBinop->GetLhs()->Access(this);
        Exp* finalDestination = mRebuiltExpression;
        const TypeDesc* child = n->GetTypeDesc()->GetChild();
        int count = n->GetTypeDesc()->GetByteSize() / child->GetByteSize();
        PG_ASSERT(lhsBinop->GetRhs()->GetExpType() == Idd::sType);
        Idd* swizzle = static_cast<Idd*>(lhsBinop->GetRhs());

        if (finalDestination->GetExpType() == Idd::sType)
        {
            Idd* finalDestinationIdd = static_cast<Idd*>(finalDestination);
            for (int i  = 0; i < count; ++i)
            {
                int destOffset = child->GetByteSize() * GetSwizzleOffset(swizzle->GetName()[i]);
                Idd* destIdd = CANON_NEW Idd(nullptr);
                destIdd->SetOffset(finalDestinationIdd->GetOffset() + destOffset);
                destIdd->SetFrameOffset(finalDestinationIdd->GetFrameOffset());
                destIdd->SetIsGlobal(finalDestinationIdd->IsGlobal());
                destIdd->SetTypeDesc(child);

                Idd* srcIdd = CANON_NEW Idd(nullptr);
                srcIdd->SetOffset(resultTemp->GetOffset() + i * child->GetByteSize());
                srcIdd->SetFrameOffset(resultTemp->GetFrameOffset());
                srcIdd->SetIsGlobal(resultTemp->IsGlobal());
                srcIdd->SetTypeDesc(child);

                PushCanon( CANON_NEW Move(destIdd, srcIdd) );
            }

        }
        else
        {
            PG_ASSERT(finalDestination->GetExpType() == Binop::sType && 
                     static_cast<Binop*>(finalDestination)->GetOp() == O_ACCESS &&
                     static_cast<Binop*>(finalDestination)->GetLhs()->GetExpType() == Idd::sType &&
                     static_cast<Binop*>(finalDestination)->GetRhs()->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT
            );
            Idd* finalDestinationArr = static_cast<Idd*>(static_cast<Binop*>(finalDestination)->GetLhs());
            Exp* finalDestinationArrIndex = static_cast<Binop*>(finalDestination)->GetRhs();

            for (int i = 0; i < count; ++i)
            {
                int destOffset = child->GetByteSize() * GetSwizzleOffset(swizzle->GetName()[i]);

                Variant v;
                v.i[0] = destOffset;
                Imm* destOffsetImm = CANON_NEW Imm(v);
                destOffsetImm->SetTypeDesc(finalDestinationArrIndex->GetTypeDesc());
                Binop* destOffsetNode = CANON_NEW Binop(finalDestinationArrIndex, O_PLUS, destOffsetImm);
                destOffsetNode->SetTypeDesc(finalDestinationArrIndex->GetTypeDesc());
                Binop* destBinop = CANON_NEW Binop(finalDestinationArr,O_ACCESS,destOffsetNode);
                destBinop->SetTypeDesc(child);

                Idd* srcIdd = CANON_NEW Idd(nullptr);
                srcIdd->SetOffset(resultTemp->GetOffset() + i * child->GetByteSize());
                srcIdd->SetFrameOffset(resultTemp->GetFrameOffset());
                srcIdd->SetIsGlobal(resultTemp->IsGlobal());
                srcIdd->SetTypeDesc(resultTemp->GetTypeDesc());

                PushCanon( CANON_NEW LoadAddr(Canon::R_C, destBinop));
                PushCanon( CANON_NEW CopyToAddr(Canon::R_C, srcIdd, srcIdd->GetTypeDesc()->GetByteSize()));
            }
    
        }

        mRebuiltExpression = newLhs;
    }
    else
    {
        mRebuiltExpression = newLhs;
    }
}

void Canonizer::HandleNonContinuousSwizzle(Exp* targetExp, Idd* swizzle, const TypeDesc* targetType)
{
    PG_ASSERT(targetExp->GetTypeDesc()->GetModifier() == TypeDesc::M_VECTOR);

    Idd* temp = AllocateTemporal(targetType);
    const TypeDesc* child = targetExp->GetTypeDesc()->GetChild();
    int countSizes = child->GetByteSize();
    int count = targetType->GetByteSize() / countSizes;

    if (targetExp->GetExpType() == Idd::sType)
    {
        Idd* element = static_cast<Idd*>(targetExp);
        for (int i = 0; i < count; ++i)
        {
            int swizzleElementOffset = GetSwizzleOffset(swizzle->GetName()[i]);

            Idd* tempOffsetted = CANON_NEW Idd(nullptr);
            tempOffsetted->SetTypeDesc(child);
            tempOffsetted->SetOffset(temp->GetOffset() + i * countSizes);
            tempOffsetted->SetFrameOffset(temp->GetFrameOffset());
            tempOffsetted->SetIsGlobal(temp->IsGlobal());

            Idd* targetOffsetted = CANON_NEW Idd(nullptr);
            targetOffsetted->SetTypeDesc(child);
            targetOffsetted->SetOffset(element->GetOffset() + swizzleElementOffset * countSizes);
            targetOffsetted->SetFrameOffset(element->GetFrameOffset());
            targetOffsetted->SetIsGlobal(element->IsGlobal());

            PushCanon( CANON_NEW Move(tempOffsetted, targetOffsetted) );
        }
    }
    else
    {
        PG_ASSERT(targetExp->GetExpType() == Binop::sType && 
                  static_cast<Binop*>(targetExp)->GetRhs()->GetTypeDesc()->GetAluEngine() == TypeDesc::E_INT &&
                  static_cast<Binop*>(targetExp)->GetOp() == O_ACCESS);

        Binop* binopLhs = static_cast<Binop*>(targetExp);

        for (int i = 0; i < count; ++i)
        {
            int swizzleElementOffset = GetSwizzleOffset(swizzle->GetName()[i]);

            Idd* tempOffsetted = CANON_NEW Idd(nullptr);
            tempOffsetted->SetTypeDesc(child);
            tempOffsetted->SetOffset(temp->GetOffset() + i * countSizes);
            tempOffsetted->SetFrameOffset(temp->GetFrameOffset());
            tempOffsetted->SetIsGlobal(temp->IsGlobal());


            Variant v;
            v.i[0] = swizzleElementOffset * countSizes;

            Imm* immOffset = CANON_NEW Imm(v);
            immOffset->SetTypeDesc(binopLhs->GetRhs()->GetTypeDesc());

            Binop* newOffset = CANON_NEW Binop(binopLhs->GetRhs(), O_PLUS, immOffset);
            newOffset->SetTypeDesc(binopLhs->GetRhs()->GetTypeDesc());

            Binop* targetOffsetted = CANON_NEW Binop(binopLhs->GetLhs(),O_ACCESS,newOffset);
            targetOffsetted->SetTypeDesc(child);

            PushCanon( CANON_NEW Move(tempOffsetted, targetOffsetted) );
        }
        
    }
    mRebuiltExpression = temp;
}

void Canonizer::HandleDotOperator(Binop* n)
{
    PG_ASSERT(n->GetOp() == O_DOT);
    PG_ASSERT(n->GetRhs()->GetExpType() == Idd::sType);

    n->GetLhs()->Access(this);
    Exp* newLhs = mRebuiltExpression;

    Idd* iddRhs = static_cast<Idd*>(n->GetRhs());
    if (newLhs->GetExpType() == Idd::sType)
    {
        Idd* iddLhs = static_cast<Idd*>(newLhs);

        PG_ASSERT(iddRhs->GetFrameOffset() == 0);
        Idd* newOffset = CANON_NEW Idd(nullptr);
        newOffset->SetFrameOffset(iddLhs->GetFrameOffset());
        newOffset->SetIsGlobal(iddLhs->IsGlobal());
        newOffset->SetTypeDesc(n->GetTypeDesc());
        mRebuiltExpression = newOffset;

        if (iddLhs->GetTypeDesc()->GetModifier() == TypeDesc::M_STRUCT)
        {
            newOffset->SetOffset(iddLhs->GetOffset() + iddRhs->GetOffset());
        }
        else
        {
            PG_ASSERT(iddLhs->GetTypeDesc()->GetModifier() == TypeDesc::M_VECTOR);
            if (IsContinuousSwizzle(iddRhs))
            {
                int swizzleOffset = GetSwizzleOffset(iddRhs->GetName()[0]);
                newOffset->SetOffset(iddLhs->GetOffset() + iddLhs->GetTypeDesc()->GetChild()->GetByteSize() * swizzleOffset);
            }
            else
            {
                //non coninuous swizzle? no problem, copy it to a temporal
                HandleNonContinuousSwizzle(iddLhs, iddRhs, n->GetTypeDesc());
            }
        }
    }
    else if (newLhs->GetExpType() == Binop::sType && (static_cast<Binop*>(newLhs)->GetOp() == O_ACCESS))
    {
        PG_ASSERT(static_cast<Binop*>(newLhs)->GetLhs()->GetTypeDesc()->GetModifier() == TypeDesc::M_ARRAY);
        
        Binop* lhsBinop = static_cast<Binop*>(newLhs);
        
        if (newLhs->GetTypeDesc()->GetModifier() == TypeDesc::M_STRUCT)
        {
            int offset = iddRhs->GetOffset();

            Variant v;
            v.i[0] = offset;
            Imm* newImm = CANON_NEW Imm(v);
            newImm->SetTypeDesc(lhsBinop->GetRhs()->GetTypeDesc());

            Binop* newOffsetExp = CANON_NEW Binop(lhsBinop->GetRhs(), O_PLUS, newImm);
            newOffsetExp->SetTypeDesc(lhsBinop->GetRhs()->GetTypeDesc());

            Binop* newBinop = CANON_NEW Binop(lhsBinop->GetLhs(), O_ACCESS, newOffsetExp);
            newBinop->SetTypeDesc(n->GetTypeDesc());
            
            mRebuiltExpression = newBinop;
        }
        else if (newLhs->GetTypeDesc()->GetModifier() == TypeDesc::M_VECTOR )
        {   
            if(IsContinuousSwizzle(iddRhs))
            {
                int offset = GetSwizzleOffset(iddRhs->GetName()[0]);

                Variant v;
                v.i[0] = offset  * newLhs->GetTypeDesc()->GetChild()->GetByteSize();
                Imm* newImm = CANON_NEW Imm(v);
                newImm->SetTypeDesc(lhsBinop->GetRhs()->GetTypeDesc());

                Binop* newOffsetExp = CANON_NEW Binop(lhsBinop->GetRhs(), O_PLUS, newImm);
                newOffsetExp->SetTypeDesc(lhsBinop->GetRhs()->GetTypeDesc());

                Binop* newBinop = CANON_NEW Binop(lhsBinop->GetLhs(), O_ACCESS, newOffsetExp);
                newBinop->SetTypeDesc(n->GetTypeDesc());
                
                mRebuiltExpression = newBinop;
            }
            else
            {
                HandleNonContinuousSwizzle(newLhs, iddRhs, n->GetTypeDesc());
            }
        }
        else
        {
            PG_FAILSTR("Unhandled access case? the BlockScriptBuilder must guard against this invalid state!");
        }
    } else {
        Idd* temporal = AllocateTemporal(newLhs->GetTypeDesc());
        PushCanon( CANON_NEW Move(temporal, newLhs));
        Binop* newBinop = CANON_NEW Binop(temporal, O_DOT, n->GetRhs());
        newBinop->SetTypeDesc(n->GetTypeDesc());
        newBinop->Access(this);
    }
}

void Canonizer::Visit(Binop* n)
{
    if (n->GetOp() == O_ARRAY_CONSTRUCTOR)
    {
        mRebuiltExpression = AllocateTemporal(n->GetTypeDesc());
    }
    else
    {
        if (n->GetOp() == O_SET)
        {
            HandleSetOperator(n);
        }
        else if (n->GetOp() == O_DOT)
        {     
            HandleDotOperator(n);
        }
        else if (n->GetOp() == O_ACCESS)
        {
            HandleArrayAccessOperator(n);
        }
        else
        {
            n->GetLhs()->Access(this);
            Exp* newLhs = mRebuiltExpression;

            n->GetRhs()->Access(this);
            mRebuiltExpression = CANON_NEW Binop(newLhs, n->GetOp(), mRebuiltExpression);
            mRebuiltExpression->SetTypeDesc(n->GetTypeDesc());
        }
    }
}

void Canonizer::Visit(Unop* unop)
{
    
    if (unop->GetOp() != O_IMPLICIT_CAST && unop->GetOp() != O_EXPLICIT_CAST)
    {
        unop->GetExp()->Access(this);
        Unop* newUnop = CANON_NEW Unop(unop->GetOp(), mRebuiltExpression);
        newUnop->SetTypeDesc(unop->GetTypeDesc());
        mRebuiltExpression = newUnop;
    }
    else
    {      
        const TypeDesc* sourceType = unop->GetExp()->GetTypeDesc();
        const TypeDesc* targetType = unop->GetTypeDesc();
        

        if (sourceType->GetAluEngine() == TypeDesc::E_INT || sourceType->GetAluEngine() == TypeDesc::E_FLOAT)
        {
            if (targetType->GetAluEngine() == TypeDesc::E_FLOAT && sourceType->GetAluEngine() == TypeDesc::E_INT)
            {
                unop->GetExp()->Access(this);
                Idd* tmp = AllocateTemporal(targetType);
                PushCanon( CANON_NEW Load(R_A, mRebuiltExpression) );
                PushCanon( CANON_NEW Cast(true, R_A) );
                PushCanon( CANON_NEW Save(tmp, R_A) );
                mRebuiltExpression = tmp;
            }
            else if (targetType->GetAluEngine() == TypeDesc::E_INT && sourceType->GetAluEngine() == TypeDesc::E_FLOAT)
            {
                unop->GetExp()->Access(this);               
                Idd* tmp = AllocateTemporal(targetType);
                PushCanon( CANON_NEW Load(R_A, mRebuiltExpression) );
                PushCanon( CANON_NEW Cast(false, R_A) );
                PushCanon( CANON_NEW Save(tmp, R_A) );
                mRebuiltExpression = tmp;
            }
            else if (targetType->GetAluEngine() >= TypeDesc::E_FLOAT2 && targetType->GetAluEngine() <= TypeDesc::E_FLOAT4)
            {
                //no need to process the internal expression since the visitor will take care of this for us.
                char* funName = mStrPool.AllocateString();
                funName[0] = '\0';
                Utils::Strcat(funName, "float");
                funName[5] = '0' + targetType->GetAluEngine() - TypeDesc::E_FLOAT2 + 2;
                funName[6] = '\0';
                //create the argument
                ExpList* arguments = CANON_NEW ExpList();
                arguments->SetExp(unop->GetExp());
                FunCall* newCall = CANON_NEW FunCall(arguments, funName);
                const FunDesc* fd = mCurrentFunTable->Find(newCall);
                PG_ASSERT(fd != nullptr);
                newCall->SetDesc(fd);
                newCall->SetTypeDesc(fd->GetDec()->GetReturnType());
                newCall->Access(this);
            }
        }
    }
}

void Canonizer::ProcessFunCall(FunCall* n)
{
    // copy this expression into a new fun call expression, with renewed exp list
    // TODO: copy constructor?
    FunCall* newFunCall = CANON_NEW FunCall(mRebuiltExpList, n->GetName());
    newFunCall->SetTypeDesc(n->GetTypeDesc());
    newFunCall->SetDesc(n->GetDesc());
    const FunDesc* funDesc = newFunCall->GetDesc();
    int funLabel = GetLabel(funDesc);
    if (funLabel == -1 && !funDesc->IsCallback())
    {
        funLabel = CreateBlock();
        RegisterFunLabel(funDesc, funLabel);
    }

    PushCanon(CANON_NEW FunGo(newFunCall, funLabel));
}

Idd* Canonizer::BeginSaveRet()
{
    Idd* tempSavRet = nullptr;
    bool shouldSaveRet = mCurrentFunDesc != nullptr && mCurrentFunDesc->GetDec()->GetReturnType()->GetByteSize() > CANON_REGISTER_BYTESIZE;

    if (shouldSaveRet)
    {
        const TypeDesc* td = mCurrentTypeTable->GetTypeDesc(mCurrentTypeTable->GetTypeByName("int"));
        tempSavRet = AllocateTemporal(td);
        PushCanon(CANON_NEW Save(tempSavRet, R_RET));
    }

    return tempSavRet;
}

void Canonizer::EndSaveRet(Idd* sav)
{
    if (sav != nullptr)
    {
        PushCanon(CANON_NEW Load(R_RET, sav));
    }
}

void Canonizer::Visit(FunCall* n)
{

    n->GetArgs()->Access(this);

    //find stack frame for editing
    // allocate (or attempt) a tempgral value
    Idd* iddTree = AllocateTemporal(n->GetTypeDesc());
    
    Idd* retTemp = BeginSaveRet();

    if (n->GetTypeDesc()->GetByteSize() > CANON_REGISTER_BYTESIZE)
    {
        PushCanon(CANON_NEW LoadAddr(R_RET, iddTree));
    }

    ProcessFunCall(n);
    
    if (n->GetTypeDesc()->GetByteSize() <= CANON_REGISTER_BYTESIZE)
    {
        PushCanon(CANON_NEW Save(iddTree, R_RET));
    }

    EndSaveRet(retTemp);

    mRebuiltExpression = iddTree;
}

void Canonizer::Visit(Imm* n)
{
    // do nothing
    mRebuiltExpression = n;
}

void Canonizer::Visit(StrImm* n)
{
    Idd* t = AllocateTemporal(n->GetTypeDesc());
    PushCanon( CANON_NEW InsertDataToHeap(t, n->GetStr()) );
    mRebuiltExpression = t;
}

void Canonizer::Visit(StmtExp* n)
{
    //process the expression, that is remove all fun calls embedded
    n->GetExp()->Access(this);
}

void Canonizer::Visit(StmtTreeModifier* n)
{
}

void Canonizer::Visit(StmtFunDec* n)
{
    //register this label
    
    int lab = GetLabel(n->GetDesc());
    if (lab == -1)
    {
        lab = CreateBlock();
        RegisterFunLabel(n->GetDesc(), lab);
    }
}

void Canonizer::Visit(StmtIfElse* n)
{
    n->GetExp()->Access(this);
    JmpCond* lastJmp = CANON_NEW JmpCond(mRebuiltExpression, 0);
    PushCanon(lastJmp);
    const StackFrameInfo* prevFrame = mCurrentStackFrame;
    mCurrentStackFrame = n->GetFrame();
    PushCanon( CANON_NEW PushFrame(n->GetFrame()) );
    n->GetStmtList()->Access(this);
    PushCanon( CANON_NEW PopFrame() );
    mCurrentStackFrame = prevFrame;
    
    int endBlock = CreateBlock();
    StmtIfElse* tail = n->GetTail();

    if (tail == nullptr)
    {
        lastJmp->SetLabel(endBlock);
    }
    else
    {
        PushCanon( CANON_NEW Jmp(endBlock));
        while (tail != nullptr)
        {
            int currentBlock = CreateBlock();
            lastJmp->SetLabel(currentBlock);
            AddBlock( currentBlock );
            if (tail->GetExp() != nullptr)
            {
                tail->GetExp()->Access(this);
                lastJmp = CANON_NEW JmpCond(mRebuiltExpression, 0);
                PushCanon(lastJmp);
            }
            else 
            {
                lastJmp = nullptr;
            }
            prevFrame = mCurrentStackFrame;
            mCurrentStackFrame = tail->GetFrame();
            PushCanon( CANON_NEW PushFrame(tail->GetFrame()) );
            tail->GetStmtList()->Access(this);
            PushCanon( CANON_NEW PopFrame() );
            mCurrentStackFrame = prevFrame;
            tail = tail->GetTail();
            if (tail != nullptr)
            {
                PushCanon( CANON_NEW Jmp(endBlock));
            }
        }
        if (lastJmp != nullptr) {
            lastJmp->SetLabel(endBlock);
        }
    }
    AddBlock( endBlock );
}

void Canonizer::Visit(StmtWhile* n)
{
    int topLabel = CreateBlock();
    int endLabel = CreateBlock();

    const StackFrameInfo* prevFrame = mCurrentStackFrame;
    mCurrentStackFrame = n->GetFrame();
    PushCanon( CANON_NEW PushFrame( n->GetFrame() ) );
    AddBlock(topLabel);
    n->GetExp()->Access(this);
    JmpCond* jmp = CANON_NEW JmpCond(mRebuiltExpression, 0);
    PushCanon( jmp );
    n->GetStmtList()->Access(this);
    PushCanon( CANON_NEW Jmp( topLabel ) );
    jmp->SetLabel(endLabel);
    AddBlock(endLabel);
    mCurrentStackFrame = prevFrame;
    PushCanon( CANON_NEW PopFrame() );
}

void Canonizer::Visit(StmtStructDef* n)
{
    // Nothing! this should be already existant :)
}

void Canonizer::Visit(StmtReturn* n)
{
    n->GetExp()->Access(this);
    const TypeDesc* typeDesc = mRebuiltExpression->GetTypeDesc();
    if (typeDesc->GetByteSize() <= CANON_REGISTER_BYTESIZE)
    {
        PushCanon( CANON_NEW Load(R_RET, mRebuiltExpression));
    }
    else
    {
        PushCanon( CANON_NEW CopyToAddr(R_RET, mRebuiltExpression, typeDesc->GetByteSize()));
    }

    //TODO: ugh improve this!!
    const StackFrameInfo* theFrame = mCurrentStackFrame;
    while (theFrame->GetCreatorCategory() != StackFrameInfo::FUN_BODY)
    {
        int parentFrame = theFrame->GetParentStackFrame();
        theFrame = &((*mCurrFrames)[parentFrame]);
        PushCanon( CANON_NEW PopFrame );
    }
    PushCanon( CANON_NEW Ret );
}
