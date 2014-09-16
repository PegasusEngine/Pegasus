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
#include "Pegasus/BlockScript/bs.parser.hpp"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Assertion.h"

#define BS_NEW PG_NEW(&mAllocator, -1, "BlockScript::Ast", Pegasus::Alloc::PG_MEM_TEMP)
#define STRING_PAGE_SIZE 512

using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;
using namespace Pegasus::Utils;


//declaration of unique type iid for expression optimization

const int Idd::sType     = 0;
const int Binop::sType   = 1;
const int FunCall::sType = 2;
const int Imm::sType     = 3;


void BlockScriptBuilder::Initialize(Pegasus::Alloc::IAllocator* allocator, BlockScriptBuilder::ErrorMsgCallback onError)
{
    mErrorCount = 0;
    mGeneralAllocator = allocator;
    mAllocator.Initialize(STRING_PAGE_SIZE, allocator);
	mStackFrames.Initialize(allocator);
    mOnError = onError;
}

void BlockScriptBuilder::BeginBuild()
{
    mCurrentFrame = 0;
    mErrorCount = 0;
    mStackFrames.PushEmpty().Initialize(mGeneralAllocator);
	mTypeTable.Initialize(mGeneralAllocator);
    mFunTable.Initialize(mGeneralAllocator);
    mActiveResult.mAst = nullptr;
}

void BlockScriptBuilder::EndBuild(BlockScriptBuilder::CompilationResult& result)
{
    result = mActiveResult;
}

void BlockScriptBuilder::Reset()
{
    mAllocator.Reset();
    mTypeTable.Shutdown();
    mStackFrames.Reset();
    mFunTable.Reset();
    mErrorCount = 0;
	mActiveResult.mAst = nullptr;
}

void BlockScriptBuilder::AttemptFunctionStackPush()
{
    //current frame 0 means we are in the root
    if (mCurrentFrame == 0)
    {
        StartNewFrame();
    }
}

void BlockScriptBuilder::StartNewFrame()
{
    StackFrameInfo& newFrame = mStackFrames.PushEmpty();
    newFrame.Initialize(mGeneralAllocator);
    newFrame.SetParentStackFrame(mCurrentFrame);
    mCurrentFrame = mStackFrames.Size() - 1;
}

void BlockScriptBuilder::PopFrame()
{
    //pop to the previous frame
    mCurrentFrame = mStackFrames[mCurrentFrame].GetParentStackFrame();
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
    AttemptFunctionStackPush();
    return BS_NEW ArgList;
}

StmtList* BlockScriptBuilder::CreateStmtList()
{
    return BS_NEW StmtList;
}

int BlockScriptBuilder::RegisterStackMember(const char* name, int type)
{
    PG_ASSERT(type != -1);
    StackFrameInfo& currentFrame = mStackFrames[mCurrentFrame];
    int offset = currentFrame.Allocate(name, type, mTypeTable);
    return offset;
}

Exp* BlockScriptBuilder::BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs)
{
    PG_ASSERT(lhs != nullptr);
    PG_ASSERT(rhs != nullptr);

    int tid1 = lhs->GetTypeId();
    int tid2 = rhs->GetTypeId();

    Binop* output = nullptr;

    if(op == O_SET) // = operator
    {
        if (tid2 == -1)
        {
            mOnError("Undefined type on = operator.");
            return nullptr;
        }
        else
        {
            if (lhs->GetExpType() == Idd::sType)
            {
                Idd* idd = static_cast<Idd*>(lhs);
                if (idd->GetOffset() == -1)
                {
                    PG_ASSERT(tid1 == -1);
                    //register into stack
                    idd->SetTypeId(tid2);
					tid1 = tid2;
                    StackFrameInfo& currentFrame = mStackFrames[mCurrentFrame];
                    int offset = currentFrame.Allocate(idd->GetName(), tid2, mTypeTable);
                    idd->SetOffset(offset);
                    idd->SetFrame(mCurrentFrame);
                }

                output = BS_NEW Binop(lhs, op, rhs);

                if (tid1 != tid2)
                {
                    mOnError("Incompatible types not allowed on operation.");
                    return nullptr;
                }
                else
                {
                    output->SetTypeId(tid1);
                }
                

            }
            else
            {
                mOnError("lhs can only be an identifier for = operator.");
                return nullptr;
            }
        }
    }
    else
    {

        output = BS_NEW Binop(lhs, op, rhs);

        if (tid1 != tid2)
        {
            mOnError("Incompatible types not allowed on operation.");
            return nullptr;
        }
        else
        {
            output->SetTypeId(tid1);
        
        }
        
    }
	return output;
    

}

Exp*   BlockScriptBuilder::BuildImmInt   (int i)
{
    Ast::Variant v;
    v.i[0] = i;
    Imm* imm = BS_NEW Imm(v);
    int typeId = mTypeTable.GetTypeByName("int");
    PG_ASSERT(typeId != -1);
    imm->SetTypeId(typeId);
    return imm;
}

Exp*   BlockScriptBuilder::BuildImmFloat   (float f)
{
    Ast::Variant v;
    v.f[0] = f;
    Imm* imm = BS_NEW Imm(v);
    int typeId = mTypeTable.GetTypeByName("float");
    PG_ASSERT(typeId != -1);
    imm->SetTypeId(typeId);
    return imm;
}

Idd*   BlockScriptBuilder::BuildIdd   (const char * name)
{
    //create the idd
    Idd* idd = BS_NEW Idd(name);

    //find type
    int currentFrame = mCurrentFrame; 
    while (currentFrame != -1)
    {
        StackFrameInfo& info = mStackFrames[currentFrame];
        int offset = -1;
        int type   = -1;
        bool found = info.FindDeclaration(name, offset, type);
        if (found) {
            idd->SetOffset(offset);
            idd->SetFrame(currentFrame);
            idd->SetTypeId(type);
            break;
        }
        currentFrame = info.GetParentStackFrame();
    }

    return idd;
}

Exp* BlockScriptBuilder::BuildFunCall(ExpList* args, const char* name)
{
    PG_ASSERT(args != nullptr);
    PG_ASSERT(name != nullptr);
    
    FunCall* fc = BS_NEW FunCall(args, name);
    int descId = mFunTable.Find(fc);
    if (descId != -1)
    {
        fc->SetDescId(descId);
		int type = mFunTable.GetDesc(descId)->GetDec()->GetReturnType();
        fc->SetTypeId(type);
        return fc;
    }
    else
    {
        mOnError("No function declaration found.");
        return nullptr;
    }
}

StmtExp* BlockScriptBuilder::BuildStmtExp(Exp* exp)
{
    PG_ASSERT(exp != nullptr);
    return BS_NEW StmtExp(exp);
}

StmtReturn* BlockScriptBuilder::BuildStmtReturn(Exp* exp)
{
    PG_ASSERT(exp != nullptr);
    return BS_NEW StmtReturn(exp);
}

StmtTreeModifier* BlockScriptBuilder::BuildStmtTreeModifier(ExpList* expList, Idd* var)
{
    PG_ASSERT(expList != nullptr);
    PG_ASSERT(var     != nullptr);
    return BS_NEW StmtTreeModifier(expList, var);
}

StmtFunDec* BlockScriptBuilder::BuildStmtFunDec(Ast::ArgList* argList, Ast::StmtList* stmtList, const char * returnIdd, const char * nameIdd)
{
    PG_ASSERT(argList != nullptr);
    PG_ASSERT(returnIdd != nullptr);
    PG_ASSERT(nameIdd != nullptr);


    //first, find the return type
    int retType = mTypeTable.GetTypeByName(returnIdd);
    if (retType == -1)
    {
        mOnError("Unknown return type.");
        return nullptr;
    }

    StmtFunDec* funDec = BS_NEW StmtFunDec(argList, stmtList, returnIdd, nameIdd);

    funDec->SetReturnType(retType);

    // record the frame for this function
    funDec->SetFrame(mCurrentFrame);

    //pop to the previous frame
    PopFrame();
    
    PG_ASSERT(mCurrentFrame != -1);

    //register in the function table
    int success = mFunTable.Insert(funDec);

    if (success == -1)
    {
        mOnError("Duplicate function declaration found.");
        return nullptr;
    }

    funDec->SetDescId(success);
    
    return funDec;
}

StmtWhile* BlockScriptBuilder::BuildStmtWhile(Exp* exp, StmtList* stmtList)
{
    PG_ASSERT(exp != nullptr);

    //pop to the previous frame
    PopFrame();
    return BS_NEW StmtWhile(exp, stmtList); 
}

StmtIfElse* BlockScriptBuilder::BuildStmtIfElse(Exp* exp, StmtList* ifBlock, ElseTail* tail)
{
    PG_ASSERT(exp != nullptr);
    PG_ASSERT(ifBlock != nullptr);
    
    //pop to the previous frame
    PopFrame();

    return BS_NEW StmtIfElse(exp, ifBlock, tail);
}

ElseIfTail* BlockScriptBuilder::BuildStmtElseIfTail(Exp* exp, StmtList* ifBlock, ElseTail* tail)
{
    PG_ASSERT(exp != nullptr);
    PG_ASSERT(ifBlock != nullptr);
    
    PopFrame();
    return BS_NEW ElseIfTail(exp, ifBlock, tail);
}

ElseTail*   BlockScriptBuilder::BuildStmtElseTail(StmtList* ifBlock)
{
    PG_ASSERT(ifBlock != nullptr);

    PopFrame();
    return BS_NEW ElseTail(ifBlock, nullptr);
}

ArgDec*  BlockScriptBuilder::BuildArgDec(const char* var, const char* type)
{
    PG_ASSERT(var != nullptr);
    PG_ASSERT(type != nullptr);

    AttemptFunctionStackPush();

    int typeId = mTypeTable.GetTypeByName(type);
    if (typeId == -1)
    {
        mOnError("Invalid type for function argument.\n");
        return nullptr;
    }

    int dummy1, dummy2;
    bool found = mStackFrames[mCurrentFrame].FindDeclaration(var, dummy1, dummy2);
    if (found)
    {
        mOnError("Duplicate name of function argument declaration\n");
        return nullptr;
    }

    int offset = RegisterStackMember(var, typeId);
    
    ArgDec* argDec = BS_NEW ArgDec(var, type);
    argDec->SetTypeId(typeId);
    argDec->SetFrame(mCurrentFrame);
    argDec->SetOffset(offset);
    
    return argDec;
}



