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


using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;
using namespace Pegasus::Utils;

#define BS_NEW PG_NEW(&mAllocator, -1, "BlockScript::Ast", Pegasus::Alloc::PG_MEM_TEMP)

void BlockScriptBuilder::Initialize(Pegasus::Alloc::IAllocator* allocator, BlockScriptBuilder::ErrorMsgCallback onError)
{
    mAllocator.Initialize(512, allocator);
    mOnError = onError;
}

void BlockScriptBuilder::BeginBuild()
{
    mActiveResult.mAst = nullptr;
}

void BlockScriptBuilder::EndBuild(BlockScriptBuilder::CompilationResult& result)
{
    result = mActiveResult;
}

void BlockScriptBuilder::Reset()
{
    mAllocator.Reset();
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

Binop* BlockScriptBuilder::BuildBinop (Ast::Exp* lhs, int op, Ast::Exp* rhs)
{
    PG_ASSERT(lhs != nullptr);
    PG_ASSERT(rhs != nullptr);

    return BS_NEW Binop(lhs, op, rhs);
}

Imm*   BlockScriptBuilder::BuildImm   (Ast::Variant& v)
{
    return BS_NEW Imm(v);
}

Idd*   BlockScriptBuilder::BuildIdd   (const char * name)
{
    return BS_NEW Idd(name);
}

FunCall* BlockScriptBuilder::BuildFunCall(ExpList* args, const char* name)
{
    PG_ASSERT(args != nullptr);
    PG_ASSERT(name != nullptr);
    return BS_NEW FunCall(args, BuildIdd(name));
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
    PG_ASSERT(stmtList != nullptr);
    PG_ASSERT(returnIdd != nullptr);
    PG_ASSERT(nameIdd != nullptr);

    return BS_NEW StmtFunDec(argList, stmtList, BuildIdd(returnIdd), BuildIdd(nameIdd));
}

StmtIfElse* BlockScriptBuilder::BuildStmtIfElse(Exp* exp, StmtList* ifBlock, StmtList* elseBlock)
{
    PG_ASSERT(exp != nullptr);
    PG_ASSERT(ifBlock != nullptr);
    PG_ASSERT(elseBlock != nullptr);
    
    return BS_NEW StmtIfElse(exp, ifBlock, elseBlock);
}

ArgDec*  BlockScriptBuilder::BuildArgDec(Idd* var, Idd* type)
{
    PG_ASSERT(var != nullptr);
    PG_ASSERT(type != nullptr);
    return BS_NEW ArgDec(var, type);
}



