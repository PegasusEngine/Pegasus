/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptBuilder.cpp
//! \author Kleber Garcia
//! \date   28th August 2014
//! \brief  Blockscript abstract syntax tree builder, also type checker. 
//!         returns respective runtime ready structures to run the script.

#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/EventListeners.h"
#include "Pegasus/BlockScript/BsIntrinsics.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Core/Assertion.h"

#define BS_NEW PG_NEW(&mAllocator, -1, "BlockScript::Ast", Pegasus::Alloc::PG_MEM_TEMP)
#define STRING_PAGE_SIZE 512

using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;
using namespace Pegasus::Utils;


//declaration of unique type iid for expression optimization

const int Idd::sType                 = 0;
const int Binop::sType               = 1;
const int FunCall::sType             = 2;
const int Imm::sType                 = 3;
const int StrImm::sType              = 4;
const int Unop::sType                = 5;
const int ArrayConstructor::sType    = 6;

//function defined in parser generator, bs.y
extern void BS_ErrorDispatcher(BlockScriptBuilder* builder, const char* message);

void BlockScriptBuilder::Initialize(Pegasus::Alloc::IAllocator* allocator)
{
    mGeneralAllocator = allocator;
    mAllocator.Initialize(STRING_PAGE_SIZE, allocator);
    mCanonizer.Initialize(allocator);
    mStrPool.Initialize(allocator);
    mEventListeners.Initialize(allocator);
    mSymbolTable.Initialize(allocator);
    mGlobalsMap.Initialize(allocator);
    mGlobalsMetaData.Initialize(allocator);
    Reset();
}

void BlockScriptBuilder::BeginBuild(const char* initialTitle)
{
    PG_ASSERTSTR(
        mErrorCount == 0 &&
        mFileStates.GetSize() == 0 &&
        mActiveResult.mAst == nullptr &&
        mActiveResult.mAsm.mBlocks == nullptr &&
        mInFunBody == false,
        "Reset() must be called prior to compiling on BlockScriptBuilder!"
    );

    for (int i = 0; i < mEventListeners.Size(); ++i)
    {
        mEventListeners[i]->OnCompilationBegin();
    }

    PushFile(initialTitle);
}

void BlockScriptBuilder::PushFile(const char* newTitle)
{
    //initialize the file line stack
    FileState& fs = mFileStates.PushEmpty();
    fs.compilationUnitTitle = newTitle;
    fs.lineNumber = 0;
}

void BlockScriptBuilder::PopFile()
{
    mFileStates.Pop();
}

int BlockScriptBuilder::GetCurrentLine() const
{
    if (mFileStates.GetSize() > 0)
    {
        return mFileStates[mFileStates.GetSize() - 1].lineNumber;
    }
    else
    {
        PG_FAILSTR("Requesting a line number without an active file state.");
        return -1;
    }
}


const char* BlockScriptBuilder::GetCurrentCompilationUnitTitle() const
{
    if (mFileStates.GetSize() > 0)
    {
        return mFileStates[mFileStates.GetSize() - 1].compilationUnitTitle;
    }
    else
    {
        PG_FAILSTR("Requesting a compilation unit name without an active file state.");
        return "<UnknownCompilationUnitTitle>";
    }
}

void BlockScriptBuilder::IncrementLine()
{
    if (mFileStates.GetSize() > 0)
    {
        ++mFileStates[mFileStates.GetSize() - 1].lineNumber;
    }
}

void BlockScriptBuilder::EndBuild(BlockScriptBuilder::CompilationResult& result)
{
    //check that all functions have a body
    if (mErrorCount == 0)
    {
        FunTable* funTable = mSymbolTable.GetRootFunTable();
        int funSize = funTable->GetSize();
        for (int f = 0; f < funSize; ++f)
        {
            FunDesc* funDesc = funTable->GetDesc(f);
            if (funDesc->GetDec()->GetStmtList() == nullptr && !funDesc->IsCallback())
            {
                ++mErrorCount;
                BS_ErrorDispatcher(this, "Missing function body.");
                break;
            }
        }
    }

    if (mErrorCount == 0)
    {

        //build of AST is done, lets canonize now (canonization process should not error out)
        mCanonizer.Canonize(
            mActiveResult.mAst,
            &mSymbolTable
        );

        mActiveResult.mAsm = mCanonizer.GetAssembly();
        mActiveResult.mAsm.mGlobalsMap = &mGlobalsMap;
    }
    else
    {
        mActiveResult.mAsm.mBlocks = nullptr;
    }

    for (int i = 0; i < mEventListeners.Size(); ++i)
    {
        mEventListeners[i]->OnCompilationEnd(mErrorCount == 0);
    }
    
    result = mActiveResult;
}

void BlockScriptBuilder::Reset()
{
    //initialize compilation state variables
    mErrorCount = 0;
    mActiveResult.mAst = nullptr;
    mActiveResult.mAsm.mBlocks = nullptr;
    mCurrAnnotations = nullptr;
    mInFunBody = false;
    mReturnTypeContext = nullptr;

    //Reset and initialize symbol containers
    mAllocator.Reset();


    mSymbolTable.Reset();
    mCurrentFrame = mSymbolTable.CreateFrame();
    mCurrentFrame->SetCreatorCategory(StackFrameInfo::GLOBAL);

    mCanonizer.Reset();
    mGlobalsMap.Reset();
    mGlobalsMetaData.Reset();
    mFileStates.Clear();

    mStrPool.Clear();

    //Event listeners must be persistent per builder instance.
}

bool BlockScriptBuilder::StartNewFunction(const TypeDesc* returnTypeContext)
{
    if (mInFunBody)
    {
        return false;
    } 
    else
    {
        mInFunBody = true;
        PG_ASSERT(mReturnTypeContext == nullptr);
        mReturnTypeContext = returnTypeContext;
        StartNewFrame();
    }
    return true;
}

StackFrameInfo* BlockScriptBuilder::StartNewFrame()
{
    StackFrameInfo* newFrame = mSymbolTable.CreateFrame();
    newFrame->SetParentStackFrame(mCurrentFrame);
    mCurrentFrame = newFrame;
    return newFrame;
}

void BlockScriptBuilder::PopFrame()
{
    //pop to the previous frame
    mCurrentFrame = mCurrentFrame->GetParentStackFrame();
}

void BlockScriptBuilder::BindIntrinsic(Ast::StmtFunDec* funDec, FunCallback callback)
{
    PG_ASSERT(mInFunBody);
    FunDesc* desc = funDec->GetDesc();
    desc->SetCallback(callback);

    PopFrame();
    mInFunBody = false;
    mReturnTypeContext = nullptr;
}

Program* BlockScriptBuilder::CreateProgram()
{
    PG_ASSERTSTR(mActiveResult.mAst == nullptr, "ensure that BeginBuild() and EndBuild() are called properly");
    mActiveResult.mAst = BS_NEW Program();
	return mActiveResult.mAst;
}

ExpList* BlockScriptBuilder::CreateExpList()
{
    return BS_NEW ExpList;
}

ArgList* BlockScriptBuilder::CreateArgList()
{
    return BS_NEW ArgList;
}

StmtList* BlockScriptBuilder::CreateStmtList()
{
    return BS_NEW StmtList;
}

const TypeDesc* BlockScriptBuilder::GetTypeByName(const char* name) const
{
    return mSymbolTable.GetTypeByName(name);
}

TypeDesc* BlockScriptBuilder::GetTypeByName(const char* name)
{
    return mSymbolTable.GetTypeForPatching(name);
}

int BlockScriptBuilder::RegisterStackMember(const char* name, const TypeDesc* type)
{
    PG_ASSERT(type != nullptr);
    int offset = mCurrentFrame->Allocate(name, type, true /*is an Arg*/);
    return offset;
}


bool BlockScriptBuilder::IsBinopValid(const TypeDesc* type, int op)
{
    if (type->GetAluEngine() == TypeDesc::E_INT)
    {
        switch(op)
        {
        case O_PLUS:
        case O_MINUS:
        case O_MUL:
        case O_DIV:
        case O_MOD:
        case O_EQ:
        case O_NEQ:
        case O_LT:
        case O_GT:
        case O_LTE:
        case O_GTE: 
        case O_LAND:
        case O_LOR:
            return true;
        }
    }
    else if (type->GetAluEngine() == TypeDesc::E_FLOAT)
    {
        switch(op)
        {
        case O_PLUS:
        case O_MINUS:
        case O_MUL:
        case O_DIV:
        case O_EQ:
        case O_NEQ:
        case O_LT:
        case O_GT:
        case O_LTE:
        case O_GTE: 
        case O_LAND:
        case O_LOR:
            return true;
        }
    }
    else
    {
        PG_ASSERTSTR(type->GetAluEngine() >= TypeDesc::E_FLOAT && type->GetAluEngine() < TypeDesc::E_COUNT, "unrecognized alu engine. Please add correct one!");
        switch(op)
        {
        case O_PLUS:
        case O_MINUS:
        case O_MUL:
        case O_DIV:
            return true;
        }
    }
    return false;
}

Exp* BlockScriptBuilder::AttemptTypePromotion(Exp* exp, const TypeDesc* targetType)
{
    const TypeDesc* currType = exp->GetTypeDesc();

    if (currType->GetAluEngine() == TypeDesc::E_INT)
    {
        if (targetType->GetAluEngine() >= TypeDesc::E_FLOAT &&
            targetType->GetAluEngine() <= TypeDesc::E_FLOAT4 
           )
        {
            Unop* unop = BS_NEW Unop(O_IMPLICIT_CAST, exp);
            unop->SetTypeDesc(targetType);
            return unop;
        }
    }
    else if (currType->GetAluEngine() == TypeDesc::E_FLOAT)
    {
        if (targetType->GetAluEngine() <= TypeDesc::E_FLOAT4 && targetType->GetAluEngine() > currType->GetAluEngine())
        {
            Unop* unop = BS_NEW Unop(O_IMPLICIT_CAST, exp);
            unop->SetTypeDesc(targetType);
            return unop;
        }
    }

    return exp;
}

Exp* BlockScriptBuilder::BuildStaticArrayDec(const TypeDesc* arrayType)
{
    if (arrayType->GetModifier() != TypeDesc::M_ARRAY)
    {
        BS_ErrorDispatcher(this, "Type passed into static_array construct must be an n dimensional array.");
        return nullptr;
    }

    //promote this node to an array constructor
    ArrayConstructor* arrayConstructor = BS_NEW ArrayConstructor();
    
    arrayConstructor->SetTypeDesc(arrayType);
    return arrayConstructor;
}

Exp* BlockScriptBuilder::BuildBinopArrayAccess (Ast::Exp* lhs, int op, Ast::Exp* rhs)
{
    PG_ASSERT(op == O_ACCESS);

    const TypeDesc* tid1 = lhs->GetTypeDesc();
    if (tid1 == nullptr)
    {
        BS_ErrorDispatcher(this, "Undefined array.");
        return nullptr;
    }

    if (tid1->GetModifier() != TypeDesc::M_ARRAY && tid1->GetModifier() != TypeDesc::M_VECTOR)
    {
        BS_ErrorDispatcher(this, "Access [] operator only allowed on array and vector types.");
        return nullptr;
    }

    const TypeDesc* tid2 = rhs->GetTypeDesc();
    if (tid2 == nullptr)
    {
        BS_ErrorDispatcher(this, "Undefined index symbol for array lookup.");
        return nullptr;
    }

    if (tid2->GetAluEngine() != TypeDesc::E_INT)
    {
        BS_ErrorDispatcher(this, "Array index must be an integer.");
        return nullptr;
    }

    Binop* newBinop = BS_NEW Binop(lhs, op, rhs);
    newBinop->SetTypeDesc(tid1->GetChild());
    return newBinop;
}

Exp* BlockScriptBuilder::BuildSetBinop(Exp* lhs, Exp* rhs, bool isExtern)
{
    const TypeDesc* tid1 = lhs->GetTypeDesc();
    const TypeDesc* tid2 = rhs != nullptr ? rhs->GetTypeDesc() : nullptr;

    Binop* output = nullptr;

    if (tid2 == nullptr)
    {
        BS_ErrorDispatcher(this, "Undefined type on = operator.");
        return nullptr;
    }
    else
    {
        if (lhs->GetExpType() == Idd::sType)
        {
            Idd* idd = static_cast<Idd*>(lhs);
            bool isDeclaration = false;
            if (idd->GetOffset() == -1)
            {
                PG_ASSERT(tid1 == nullptr);
                //register into stack
                idd->SetTypeDesc(tid2);
				tid1 = tid2;
                //TODO: simplify this
                int offset = mCurrentFrame->Allocate(idd->GetName(), tid2);
                idd->SetOffset(offset);
                idd->SetFrameOffset(0);
                isDeclaration = true;
            }
            else
            {
                rhs = AttemptTypePromotion(rhs, tid1);
                tid2 = rhs->GetTypeDesc();
                PG_ASSERT(tid2 != nullptr);
            }

            if (isExtern)
            { 
                if (!idd->GetMetaData().isGlobal)
                {
                    BS_ErrorDispatcher(this, "Can only use extern keyword on global variables.");
                    return nullptr;
                }

                if (!isDeclaration)
                {
                    BS_ErrorDispatcher(this, "cannot use extern keyword on an identifier already declared.");
                    return nullptr;
                }

                idd->GetMetaData().isExtern = isExtern;

                Imm* imm = nullptr;
                if (rhs->GetExpType() != Imm::sType)
                {
                    BS_ErrorDispatcher(this, "Right hand side of extern declaration must be an immediate value, evaluated at compile time.");
                    return nullptr;
                }

                imm = static_cast<Imm*>(rhs);

                RegisterExternGlobal(idd, imm);
            }

            output = BS_NEW Binop(lhs, O_SET, rhs);

            if (!tid1->Equals(tid2))
            {
                BS_ErrorDispatcher(this, "Incompatible types not allowed on operation.");
                return nullptr;
            }
            else
            {
                output->SetTypeDesc(tid1);
            }

        }
        else if (lhs->GetExpType() == Binop::sType && (static_cast<Binop*>(lhs)->GetOp() == O_DOT || static_cast<Binop*>(lhs)->GetOp() == O_ACCESS))
        {
            if (isExtern)
            {
                BS_ErrorDispatcher(this, "cannot use extern keyword on an identifier already declared.");
                return nullptr;
            }

            if (tid1 == nullptr || tid2 == nullptr)
            {
                BS_ErrorDispatcher(this, "undefined member on = operator.");
                return nullptr;
            }
    
            rhs = AttemptTypePromotion(rhs, tid1);
            tid2 = rhs->GetTypeDesc();

            if (!tid1->Equals(tid2))
            {
                BS_ErrorDispatcher(this, "incompatible types on = operator.");
                return nullptr;
            }

            output = BS_NEW Binop(lhs, O_SET, rhs);
            output->SetTypeDesc(tid1);

            return output;

        }
        else
        {
            BS_ErrorDispatcher(this, "lhs can only be an accessor operation.");
            return nullptr;
        }
    }
    return output;
}

Exp* BlockScriptBuilder::BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs)
{
    PG_ASSERT(lhs != nullptr);
    PG_ASSERT(rhs != nullptr);

    const TypeDesc* tid1 = lhs->GetTypeDesc();
    const TypeDesc* tid2 = rhs != nullptr ? rhs->GetTypeDesc() : nullptr;

    Binop* output = nullptr;

    if(op == O_SET)
    {
        return BuildSetBinop(lhs, rhs);
    }
    else if (op == O_DOT)
    {
        PG_ASSERT(rhs->GetExpType() == Idd::sType);
        Idd* accessOffset = static_cast<Idd*>(rhs);
        accessOffset->SetFrameOffset(0);//no weird frames
        accessOffset->GetMetaData().isGlobal = false;//not a global
        if (tid1 == nullptr)
        {
            BS_ErrorDispatcher(this, "undefined lhs of dot operator");
            return nullptr;
        }
        
        if (tid1->GetModifier() == TypeDesc::M_STRUCT)
        {
            //find the offset of this type.
            const StmtStructDef* structDef = tid1->GetStructDef();
            PG_ASSERT(structDef != nullptr);
            
            ArgList* argList = structDef->GetArgList();
            
            tid2 = nullptr;
            int offset = 0;
            while (argList != nullptr && argList->GetArgDec() != nullptr)
            {
                if (!Pegasus::Utils::Strcmp(argList->GetArgDec()->GetVar(), accessOffset->GetName()))
                {
                    tid2 = argList->GetArgDec()->GetType();
                    PG_ASSERT(tid2 != nullptr);
                    break;
                }
                offset += argList->GetArgDec()->GetType()->GetByteSize();
                argList = argList->GetTail();
            }

            accessOffset->SetOffset(offset);

            if (tid2 == nullptr)
            {
                BS_ErrorDispatcher(this, "Member of struct not found.");
            }

            output = BS_NEW Binop(lhs, op, rhs);
            output->SetTypeDesc(tid2);
            return output;
        }
        else if (tid1->GetModifier() == TypeDesc::M_VECTOR)
        {
            if (lhs->GetExpType() == Binop::sType
                && (static_cast<Binop*>(lhs)->GetOp() == O_DOT &&
                    static_cast<Binop*>(lhs)->GetLhs()->GetTypeDesc()->GetModifier() == TypeDesc::M_VECTOR)
                )
            {
                BS_ErrorDispatcher(this, "Nested swizzle access not allowed.");
                return nullptr;
            }

            const char* swizzleName = accessOffset->GetName();
            int swizzleLen = Utils::Strlen(swizzleName);
            if (swizzleLen > 4)
            {
                BS_ErrorDispatcher(this, "Vector swizzle too long");
                return nullptr;
            }

            int dimensionality = tid1->GetModifier() == TypeDesc::M_SCALAR ? 1 : tid1->GetModifierProperty().VectorSize;

            int maxChar = -1;
            for (int i = 0; i < swizzleLen; ++i)
            {
                if (swizzleName[i] < 'w' || swizzleName[i] > 'z')
                {
                    BS_ErrorDispatcher(this, "vector access can only be 'x', 'y', 'z', 'w'");
                    return nullptr;
                }

                int currElement = (swizzleName[i] - 'w' + 3) % 4;
                maxChar = currElement > maxChar ? currElement : maxChar;
            }

            if (maxChar >= dimensionality)
            {
                BS_ErrorDispatcher(this, "Invalid subscript, make sure the vector's dimensions are accessed correctly.");
                return nullptr;
            }

            char* newName = mStrPool.AllocateString();
            newName[0] = '\0';
            Utils::Strcat(newName, tid1->GetChild()->GetName());
            if (swizzleLen >= 2)
            {
                char str[2] = {((char) swizzleLen + '0'), '\0'};
                Utils::Strcat(newName, str);
            }

            const TypeDesc* newType = mSymbolTable.GetTypeByName(newName);
            if (newType == nullptr)
            {
                BS_ErrorDispatcher(this, "Complex swizzle not allowed for this type.");
                return nullptr;
            }

            Binop* newBinop = BS_NEW Binop(
                lhs, op, rhs);
            newBinop->SetTypeDesc(newType);
            return newBinop;

        }
        else if (tid1->GetModifier() == TypeDesc::M_REFERECE)
        {
            tid2 = nullptr;
            PG_ASSERT(rhs->GetExpType() == Idd::sType);
            Idd* idd = static_cast<Idd*>(rhs);
            const char* propertyName = idd->GetName();
            const PropertyNode* propertyList = tid1->GetPropertyNode();
            if (propertyList == nullptr)
            {
                BS_ErrorDispatcher(this, "Object has no properties registered.");
                return nullptr;
            }
            
            while (propertyList != nullptr)
            {
                if (!Utils::Strcmp(propertyList->mName, propertyName))
                {
                    //found the property! lets fill in the type of this expression
                    const TypeDesc* expType = propertyList->mType;
                    Binop* newBinop = BS_NEW Binop(lhs, op, rhs);
                    newBinop->SetTypeDesc(expType);
                    return newBinop;
                }
                else
                {
                    propertyList = propertyList->mNext;
                }
            }

            BS_ErrorDispatcher(this, "Property not found for object.");
            return nullptr;
        }
        else
        {
            BS_ErrorDispatcher(this, "dot operator can only be executed in structs types");
            return nullptr;
        }
    }
    else if (op == O_ACCESS)
    {
        return BuildBinopArrayAccess(lhs, op, rhs);
    }
    else
    {
        if (tid1 == nullptr || tid2  == nullptr)
        {
            BS_ErrorDispatcher(this, "undefined symbol.");
            return nullptr;
        }

        lhs = AttemptTypePromotion(lhs, tid2); 
        tid1 = lhs->GetTypeDesc();

        rhs = AttemptTypePromotion(rhs, tid1); 
        tid2 = rhs->GetTypeDesc();

        if (!tid1->Equals(tid2))
        {
            BS_ErrorDispatcher(this, "Incompatible types not allowed on operation.");
            return nullptr;
        }

        if (!IsBinopValid(tid1, op))
        {
            BS_ErrorDispatcher(this, "Arithmetic operation is invalid for type.");
            return nullptr;
        }

        output = BS_NEW Binop(lhs, op, rhs);
        output->SetTypeDesc(tid1);
        
    }
	return output;
}

Exp* BlockScriptBuilder::BuildUnopPost(Exp* exp, int op)
{
    PG_ASSERTSTR(op == O_INC || op == O_DEC, "%d", op);
    Exp* unop = BuildUnop(op, exp);
    if (unop != nullptr)
    {
        static_cast<Unop*>(unop)->SetIsPost(true); 
    }
    return unop;
}


Exp* BlockScriptBuilder::BuildUnop(int op, Exp* exp)
{
    if (exp->GetTypeDesc() == nullptr)
    {
        BS_ErrorDispatcher(this, "Undefined unary operator on element.");
        return nullptr;
    }

    if (op == O_MINUS && exp->GetExpType() == Imm::sType)
    {
        Imm* imm = static_cast<Imm*>(exp);
        switch (exp->GetTypeDesc()->GetAluEngine())
        {
        case TypeDesc::E_INT:
            imm->GetVariant().i[0] = -imm->GetVariant().i[0];
            return imm;
        case TypeDesc::E_FLOAT4:
            imm->GetVariant().f[3] = -imm->GetVariant().f[3];
        case TypeDesc::E_FLOAT3:
            imm->GetVariant().f[2] = -imm->GetVariant().f[2];
        case TypeDesc::E_FLOAT2:
            imm->GetVariant().f[1] = -imm->GetVariant().f[1];
        case TypeDesc::E_FLOAT:
            imm->GetVariant().f[0] = -imm->GetVariant().f[0];
            return imm;
        default:
            break;
        }
    }
    else if (op == O_DEC || op == O_INC)
    {
        if (exp->GetTypeDesc()->GetAluEngine() != TypeDesc::E_INT)
        {
            BS_ErrorDispatcher(this, "Post/Pre increment or decrement only allowed on ints.");
            return nullptr;
        }

        if (exp->GetExpType() != Idd::sType)
        {
            BS_ErrorDispatcher(this, "Pre inc and post inc only allowed on a variable.");
            return nullptr;
        }
    }

    Exp* unop = BS_NEW Unop(op, exp);
    unop->SetTypeDesc(exp->GetTypeDesc());


    return unop;
}

Exp* BlockScriptBuilder::BuildExplicitCast(Exp* exp, const TypeDesc* type)
{
    if (exp->GetTypeDesc() == nullptr)
    {
        BS_ErrorDispatcher(this, "Undefined element being casted.");
        return nullptr;
    }
    //TODO: revise all potential paths of explicit casts.
    Unop* unop = BS_NEW Unop(O_EXPLICIT_CAST, exp);        
    unop->SetTypeDesc(type);

    return unop;
}

Exp*   BlockScriptBuilder::BuildImmInt   (int i)
{
    Ast::Variant v;
    v.i[0] = i;
    Imm* imm = BS_NEW Imm(v);
    const TypeDesc* typeDesc = GetTypeByName("int");
    PG_ASSERT(typeDesc != nullptr);
    imm->SetTypeDesc(typeDesc);
    return imm;
}

const char* BlockScriptBuilder::AllocStrImm(const char* strToCopy)
{
    StrImm* strImm = static_cast<StrImm*>(BlockScriptBuilder::BuildStrImm(strToCopy));
    if (strImm != nullptr)
    {
        return strImm->GetStr();
    }
    else
    {
        return nullptr;
    }    
}

void BlockScriptBuilder::RegisterExternGlobal(Ast::Idd* var, Ast::Imm* defaultVal)
{
    GlobalMapEntry& entry = mGlobalsMap.PushEmpty();
    entry.mVar = var;
    entry.mDefaultVal = defaultVal;

    mGlobalsMetaData.PushEmpty() = &(var->GetMetaData());
}

Exp*  BlockScriptBuilder::BuildStrImm(const char* strToCopy)
{

    int len = Utils::Strlen(strToCopy);
    if (len >= mAllocator.GetPageSize())
    {
        BS_ErrorDispatcher(this, "String immediate value too big.");
        return nullptr;
    } 

    char* destStr = static_cast<char*>(mAllocator.Alloc(len + 1, Pegasus::Alloc::PG_MEM_PERM));
    
    if (destStr == nullptr)
    {
        BS_ErrorDispatcher(this, "internal compiler error while allocting imm string");
        return nullptr;
    }

    destStr[0] = '\0';
    Utils::Strcat(destStr, strToCopy);

    const TypeDesc* strTypeDesc = GetTypeByName("string");
    PG_ASSERT(strTypeDesc != nullptr);

    StrImm* strImm = BS_NEW StrImm(destStr);
    strImm->SetTypeDesc(strTypeDesc);

    return strImm;
}

Exp*   BlockScriptBuilder::BuildImmFloat   (float f)
{
    Ast::Variant v;
    v.f[0] = f;
    Imm* imm = BS_NEW Imm(v);
    const TypeDesc* typeDesc = GetTypeByName("float");
    PG_ASSERT(typeDesc != nullptr);
    imm->SetTypeDesc(typeDesc);
    return imm;
}

Exp*   BlockScriptBuilder::BuildIdd   (const char * name)
{
    //see if this id is actually an enumeration
    const EnumNode* enumNode = nullptr;
    const TypeDesc* enumType = nullptr;
    bool found = mSymbolTable.FindEnumByName(name, &enumNode, &enumType);
    if (found)
    {
        Variant v;
        v.i[0] = enumNode->mGuid;
        Imm* enumVal = BS_NEW Imm(v);
        enumVal->SetTypeDesc(enumType);
        return enumVal;
    }
    else if (IsInAnnotation())
    {
        //if this is an annotation, just create an idd.
        return BS_NEW Idd(name);
    }
    else
    {
        PG_ASSERT(enumNode == nullptr && enumType == nullptr);

        //create the idd
        Idd* idd = BS_NEW Idd(name);

        //find type
        StackFrameInfo* currentFrame = mCurrentFrame; 
        int frameOffset = 0;
        while (currentFrame != nullptr)
        {
            StackFrameInfo::Entry* found = currentFrame->FindDeclaration(name);
            if (found != nullptr) {
                idd->SetOffset(found->mOffset);
                idd->SetFrameOffset(frameOffset);
                idd->SetTypeDesc(found->mType);
                idd->GetMetaData().isGlobal = (currentFrame->GetParentStackFrame() == nullptr);
                break;
            }
            currentFrame = currentFrame->GetParentStackFrame();
            frameOffset++;
        }

        //is this an undeclared idd/not found? see if its a global
        if (idd->GetOffset() == -1)
        {
            idd->GetMetaData().isGlobal = (mCurrentFrame->GetParentStackFrame() == nullptr);
        }
        else
        {
            //if its actually found, and if its a global, is it used in the global scope?
            if (idd->GetMetaData().isGlobal)
            {
                //find it in the extern definitions
                for (int i = 0; i < mGlobalsMap.Size(); ++i)
                {
                    if (idd->GetOffset() == mGlobalsMap[i].mVar->GetOffset() &&
                        !mGlobalsMap[i].mVar->GetMetaData().isUsedInGlobalScope
                     )
                    {
                        mGlobalsMetaData[i]->isUsedInGlobalScope = !mInFunBody;
                        break;
                    }
                }
            }
        }

        return idd;
    }

    return nullptr;
}


StmtExp* BlockScriptBuilder::BuildExternVariable(Ast::Exp* lhs, Ast::Exp* rhs)
{
    Exp * exp = BuildSetBinop(lhs, rhs, true /*isExtern*/);
    if (exp != nullptr)
    {
        return BuildStmtExp(exp);
    }
    
    return nullptr;
}

Annotations* BlockScriptBuilder::BeginAnnotations()
{   
    if (mCurrentFrame->GetParentStackFrame() != nullptr)
    {
        BS_ErrorDispatcher(this, "Annotations are only allowed in the global scope.");
        return nullptr;
    }

    mCurrAnnotations = BS_NEW Annotations();
    return mCurrAnnotations;
}

Annotations* BlockScriptBuilder::EndAnnotations(Annotations* annotations, ExpList* expList)
{
    PG_ASSERT(annotations == mCurrAnnotations);
    annotations->SetExpList(expList);

    //process the annotation list. Only accept:
    //an Idd = Imm or
    //an Idd = StrImm

    if (expList == nullptr || expList->GetExp() == nullptr)
    {
        BS_ErrorDispatcher(this, "Must declare at least 1 annotation.");
        return nullptr;
    }

    ExpList* currEl = expList;
    while (currEl != nullptr && currEl->GetExp() != nullptr)
    {
        Exp* currExp = currEl->GetExp();
        if (
            currExp->GetExpType() == Binop::sType 
        &&  static_cast<Binop*>(currExp)->GetLhs()->GetExpType() == Idd::sType
        &&  ( 
                static_cast<Binop*>(currExp)->GetRhs()->GetExpType() == Imm::sType
            ||  static_cast<Binop*>(currExp)->GetRhs()->GetExpType() == StrImm::sType
            )
        )
        {
            static_cast<Binop*>(currExp)->GetLhs()->SetTypeDesc(
                static_cast<Binop*>(currExp)->GetRhs()->GetTypeDesc()
            );
        }
        else
        {
            BS_ErrorDispatcher(this, "Annotations only allow an identifier with an immediate value or a string value.");
            return nullptr;
        }
        currEl = currEl->GetTail();
    }

    mCurrAnnotations = nullptr;
    return annotations;
}


StmtExp* BlockScriptBuilder::BuildDeclarationWithAnnotation(Ast::Annotations* ann, Ast::Exp* exp)
{
    if (exp->GetExpType() == Binop::sType &&
        static_cast<Binop*>(exp)->GetOp() == O_SET &&
        static_cast<Binop*>(exp)->GetLhs()->GetExpType() == Idd::sType
       )
    {
        static_cast<Idd*>(static_cast<Binop*>(exp)->GetLhs())->SetAnnotations(ann);
    }

    return BuildStmtExp(exp);
}

FunDesc* BlockScriptBuilder::FindFunctionDescription(FunCall* fcSignature)
{
    return mSymbolTable.FindFunctionDescription(fcSignature);
}

Exp* BlockScriptBuilder::BuildMethodCall(Exp* exp, const char* name, ExpList* args)
{
    ExpList* newList = BS_NEW ExpList();
    newList->SetExp(exp);
    newList->SetTail(args);
    return BuildFunCall(newList, name, true);
}

Exp* BlockScriptBuilder::BuildFunCall(ExpList* args, const char* name, bool isMethod)
{
    PG_ASSERT(args != nullptr);
    PG_ASSERT(name != nullptr);

    //check that all args have a type. otherwise bail.
    ExpList* tail = args;
    while(tail != nullptr && tail->GetExp() != nullptr)
    {
        if (tail->GetExp()->GetTypeDesc() == nullptr)
        {
            BS_ErrorDispatcher(this, "undefined parameter passed to function call.");
            return nullptr;
        }
        tail = tail->GetTail();
    }
    
    FunCall* fc = BS_NEW FunCall(args, name);
    fc->SetIsMethod(isMethod);
    FunDesc* desc = FindFunctionDescription(fc);
    if (desc != nullptr)
    {
        fc->SetDesc(desc);
		const TypeDesc* type = desc->GetDec()->GetReturnType();
        fc->SetTypeDesc(type);
        Exp* finalExp = fc;
        // compile time resolutions
        for (int i = 0; i < mEventListeners.Size(); ++i)
        {
            finalExp = mEventListeners[i]->OnResolveFunCall(&mAllocator, fc);
            if (finalExp->GetExpType() == FunCall::sType)
            {
                fc = static_cast<FunCall*>(finalExp);
            }
            else
            {
                break;
            }
        }
        return finalExp;
    }
    else
    {
        BS_ErrorDispatcher(this, "No function declaration found.");
        return nullptr;
    }
}

StmtExp* BlockScriptBuilder::BuildStmtExp(Exp* exp)
{
    PG_ASSERT(exp != nullptr);
    if (exp->GetExpType() == Binop::sType)
    {
        Binop* binop = static_cast<Binop*>(exp);
        if (binop->GetOp() != O_SET)
        {
            BS_ErrorDispatcher(this, "Empty expressions not allowed! value must be stored in a variable");
            return nullptr;
        }
    }
    else if (exp->GetExpType() == Unop::sType)
    {
        Unop* unop = static_cast<Unop*>(exp);
        if (unop->GetOp() != O_DEC && unop->GetOp() != O_INC)
        {
            BS_ErrorDispatcher(this, "Empty unary expressions not allowed, unless is post/pre increment/decrement.");
            return nullptr;
        }
    }
    else if (exp->GetExpType() != FunCall::sType)
    {
        BS_ErrorDispatcher(this, "Empty expressions not allowed! expression must be a function call, did you forget passing parameters ?");
        return nullptr;
    }
    return BS_NEW StmtExp(exp);
}

StmtReturn* BlockScriptBuilder::BuildStmtReturn(Exp* exp)
{
    PG_ASSERT(exp != nullptr);
    if (exp->GetTypeDesc() == nullptr)
    {
        BS_ErrorDispatcher(this, "Identifier not found for return statement.");
        return nullptr;
    }
    if (mInFunBody == false)
    {
        BS_ErrorDispatcher(this, "return statements not allowed on global scope.");
        return nullptr;
    }

    if (!exp->GetTypeDesc()->Equals(mReturnTypeContext))
    {
        BS_ErrorDispatcher(this, "return type must match that of the current function context.");
        return nullptr;
    }
    return BS_NEW StmtReturn(exp);
}

FunDesc* BlockScriptBuilder::RegisterFunctionDeclaration(Ast::StmtFunDec* funDec)
{
    return mSymbolTable.CreateFunctionDescription(funDec);
}

StmtFunDec* BlockScriptBuilder::BuildStmtFunDec(Ast::ArgList* argList, const TypeDesc* returnType, const char * nameIdd)
{
    PG_ASSERT(nameIdd != nullptr);


    StmtFunDec* funDec = BS_NEW StmtFunDec(argList, returnType, nameIdd);

    // record the frame for this function
    funDec->SetFrame(mCurrentFrame);

    mCurrentFrame->SetCreatorCategory(StackFrameInfo::FUN_BODY);

    
    PG_ASSERT(mCurrentFrame != nullptr);

    //register in the function table
    FunDesc* description = RegisterFunctionDeclaration(funDec);

    if (description == nullptr)
    {
        BS_ErrorDispatcher(this, "Duplicate function declaration found.");
        return nullptr;
    }

    funDec->SetDesc(description);

    
    return funDec;
}

StmtFunDec* BlockScriptBuilder::BindFunImplementation(StmtFunDec* funDec, StmtList* stmts)
{
    PG_ASSERT (funDec != nullptr);

    funDec->SetStmtList(stmts);

    //pop to the previous frame
    mInFunBody = false;
    mReturnTypeContext = nullptr;


    PopFrame();

    return funDec;
}

StmtWhile* BlockScriptBuilder::BuildStmtWhile(Exp* exp, StmtList* stmtList)
{
    PG_ASSERT(exp != nullptr);

    StmtWhile* stmtWhile = BS_NEW StmtWhile(exp, stmtList);

    stmtWhile->SetFrame(mCurrentFrame);

    mCurrentFrame->SetCreatorCategory(StackFrameInfo::LOOP);

    //pop to the previous frame
    PopFrame();

    return stmtWhile; 
}

StmtFor* BlockScriptBuilder::BuildStmtFor(Exp* init, Exp* cond, Exp* update, StmtList* stmtList)
{
    if (cond != nullptr && cond->GetTypeDesc() != nullptr && cond->GetTypeDesc()->GetAluEngine() != TypeDesc::E_INT)
    {
        BS_ErrorDispatcher(this, "Only allowed to have type int for conditional of for loop.");
        return nullptr;
    }

    StmtFor* stmtFor = BS_NEW StmtFor(init, cond, update, stmtList);
    stmtFor->SetFrame(mCurrentFrame);

    //pop the previous frame
    PopFrame();

    return stmtFor;
}

StmtIfElse* BlockScriptBuilder::BuildStmtIfElse(Exp* exp, StmtList* ifBlock, StmtIfElse* tail, StackFrameInfo* frame)
{
    if (exp != nullptr && exp->GetTypeDesc() == nullptr)//else statement does not have an exp*
    {
        BS_ErrorDispatcher(this, "Expression inside if statement not defined.");
        return nullptr;
    }
    StmtIfElse* stmtIfElse = BS_NEW StmtIfElse(exp, ifBlock, tail, frame);

    mCurrentFrame->SetCreatorCategory(StackFrameInfo::IF_STMT);

    return stmtIfElse;
}

static bool TestStringLength(const char * str)
{
    if (Strlen(str) + 1 > IddStrPool::sCharsPerString)
    {
        return false;
    }
    return true;
}

char* BlockScriptBuilder::CopyString(const char* strIn)
{
    PG_ASSERT (Strlen(strIn)  < IddStrPool::sCharsPerString)
    char* newStr = GetStringPool().AllocateString();
    newStr[0] = '\0';
    return Utils::Strcat(newStr, strIn);
}

void BlockScriptBuilder::CreateIntrinsicFunction(const char* funName, const char* const* argTypes, const char* const* argNames, int argCount, const char* returnType, FunCallback callback, bool isMethod)
{
    //step 1, check that strings and types exist.
    for (int i = 0; i < argCount; ++i)
    {
        const char* argType = argTypes[i];
        const char* argName = argNames[i];
        if (!TestStringLength(argType) || !TestStringLength(argName))
        {
            PG_FAILSTR("length of argument type %s or name %s is too big.", argType, argName);
            return;
        }

        //test types exist
        if (GetTypeByName(argType) == nullptr)
        {
            PG_FAILSTR("Type '%s' not found in current context.", argType);
            return;
        }
    }

    if (!TestStringLength(returnType) || !TestStringLength(funName))
    {
        PG_FAILSTR("The length of the return type is too big.");
        return;
    }
    const TypeDesc* returnTypeDesc = GetTypeByName(returnType);
    if (returnTypeDesc == nullptr)
    {
        PG_FAILSTR("Type %s not found in current context", returnType);
    }

    //step 2, setup the argument description of this function
    StartNewFunction(returnTypeDesc);
    Ast::ArgList* argList = nullptr;
    Ast::ArgList* currNode = nullptr;
    for (int i = 0; i < argCount; ++i)
    {
        char* argTypeCpy = CopyString(argTypes[i]);
        char* argNameCpy = CopyString(argNames[i]);
        const TypeDesc* currType = GetTypeByName(argTypeCpy);
        PG_ASSERT(currType != nullptr);
        if (argList == nullptr)
        {
            argList = CreateArgList();
            currNode = argList;
        }
        else
        {
            currNode->SetTail( CreateArgList());
            currNode = currNode->GetTail();
        }

        ArgDec* argDec = BuildArgDec(argNameCpy, currType);
        PG_ASSERT(argDec != nullptr);
        currNode->SetArgDec(argDec);
    }

    char* funNameCpy = CopyString(funName);


    //step 3, build the statement
    StmtFunDec* funDec = BuildStmtFunDec(argList, returnTypeDesc, funNameCpy);
    
    if (funDec == nullptr)
    {
        PG_FAILSTR("failed creating intrinsic function %s", funNameCpy);
    }
    
    funDec->GetDesc()->SetIsMethod(isMethod);
    BindIntrinsic(funDec, callback);
}

static void StructGenericConstructor(FunCallbackContext& ctx)
{
    void* argin = ctx.GetRawInputBuffer();
    void* argout = ctx.GetRawOutputBuffer();
    int arginByteSize = ctx.GetInputBufferSize();
    int argoutByteSize = ctx.GetOutputBufferSize();
    if (arginByteSize != 0)
    {
        PG_ASSERT(arginByteSize == argoutByteSize);
        Pegasus::Utils::Memcpy(argout, argin, argoutByteSize);
    }
    else
    {
        Pegasus::Utils::Memset8(argout, 0, argoutByteSize);
    }
}

StmtEnumTypeDef* BlockScriptBuilder::BuildStmtEnumTypeDef(const TypeDesc* typeDesc)
{
    PG_ASSERT(typeDesc != nullptr);
    return BS_NEW StmtEnumTypeDef(typeDesc);
}

StmtStructDef* BlockScriptBuilder::BuildStmtStructDef(const char* name, ArgList* definitions)
{
    PG_ASSERT(definitions != nullptr);

    //first make sure that there is no struct redefinition.
    if (GetTypeByName(name) != nullptr)
    {
        BS_ErrorDispatcher(this, "Struct type is trying to redefine a type.");
        return nullptr;
    }

    if (mInFunBody)
    {
        BS_ErrorDispatcher(this, "Struct definitions not allowed inside function bodies.");
        return nullptr;
    }
    
    StmtStructDef* newDef = BS_NEW StmtStructDef(name, definitions);

    StackFrameInfo* frameInfo = mCurrentFrame;

    newDef->SetFrameInfo(frameInfo);

    frameInfo->SetCreatorCategory(StackFrameInfo::STRUCT_DEF);

    //pop to the previous frame
    PopFrame();

    // unlink this to any parent, preventing searches on parent frames. this is now an orphan stack frame
    frameInfo->UnlinkParentStackFrame();

    TypeDesc* newStructType = mSymbolTable.CreateStructType(
        name,
        newDef //register this types structural definition AST member
    );

    //Create empty constructor
    CreateIntrinsicFunction(
        name,
        nullptr, //no argins types
        nullptr, //no argins names
        0, //no argcounts
        name,
        StructGenericConstructor
    );
    
    static const int MAX_CHILD_MEMBERS = 255;

    static const char* sMassiveCharTypeContainer[MAX_CHILD_MEMBERS];
    static const char* sMassiveCharNameContainer[MAX_CHILD_MEMBERS];

    int count = 0;
    ArgList* argList = definitions;
    while (argList != nullptr && argList->GetArgDec() != nullptr)
    {
        if (count >= MAX_CHILD_MEMBERS)
        {
            BS_ErrorDispatcher(this, "Too many members in structure");
            return nullptr;
        }

        sMassiveCharNameContainer[count] = argList->GetArgDec()->GetVar();
        sMassiveCharTypeContainer[count] = argList->GetArgDec()->GetType()->GetName();
        ++count;
        argList = argList->GetTail();
    }

    //Create constructor
    CreateIntrinsicFunction(
        name,
        sMassiveCharTypeContainer, //no argins types
        sMassiveCharNameContainer, //no argins names
        count, //no argcounts
        name,
        StructGenericConstructor
    );

    //copy all the declaration info
    

    if (newStructType == nullptr)
    {
        BS_ErrorDispatcher(this, "An error occured creating the type for this struct.");
        return nullptr;
    }
    
    return newDef;
    
}

ArgDec*  BlockScriptBuilder::BuildArgDec(const char* var, const TypeDesc* type)
{
    PG_ASSERT(var != nullptr);
    PG_ASSERT(type != nullptr);

    StackFrameInfo::Entry* found = mCurrentFrame->FindDeclaration(var);
    if (found != nullptr)
    {
        BS_ErrorDispatcher(this, "Duplicate name of declaration\n");
        return nullptr;
    }

    int offset = RegisterStackMember(var, type);
    
    ArgDec* argDec = BS_NEW ArgDec(var, type);
    argDec->SetOffset(offset);
    
    return argDec;
}



