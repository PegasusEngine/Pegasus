/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SymbolTable.cpp
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Master recursive table of functions and types. Use this to have reusable components
//!         across blockscript libraries

#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

SymbolTable::SymbolTable()
    : mAllocator(nullptr)
{
}

SymbolTable::~SymbolTable()
{
}

void SymbolTable::Initialize(Pegasus::Alloc::IAllocator* allocator)
{
    mAllocator = allocator;
    mFunTable.Initialize(allocator);
    mTypeTable.Initialize(allocator);
    mFrames.Initialize(allocator);
    mChildren.Initialize(allocator);
}

void SymbolTable::RegisterChild(SymbolTable* symbolTable)
{
    SymbolTable** newSpace = &mChildren.PushEmpty();
    *newSpace = symbolTable;
}

void SymbolTable::Reset()
{
    mFunTable.Reset();
    mTypeTable.Shutdown();
    mFrames.Reset();
    mChildren.Reset();
}

const TypeDesc* SymbolTable::GetTypeByName(const char* typeName) const
{
    //first find it recursively on the children symbol tables
    int childCount = mChildren.Size();
    for (int i = 0; i < childCount; ++i)
    {
        const TypeDesc* type = mChildren[i]->GetTypeByName(typeName);
        if (type != nullptr)
        {
            return type;
        }
    }
    return mTypeTable.GetTypeByName(typeName);
}

TypeDesc* SymbolTable::GetTypeForPatching(const char* typeName)
{
    //first find it recursively on the children symbol tables
    int childCount = mChildren.Size();
    for (int i = 0; i < childCount; ++i)
    {
        TypeDesc* type = mChildren[i]->GetTypeForPatching(typeName);
        if (type != nullptr)
        {
            return type;
        }
    }
    return mTypeTable.GetTypeForPatching(typeName);
}

TypeDesc* SymbolTable::CreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    const TypeDesc* child,
    int modifierProperty,
    TypeDesc::AluEngine engine,
    BlockScript::Ast::StmtStructDef* structDef,
    TypeDesc::EnumNode* enumNode 
)
{
    return mTypeTable.CreateType(modifier, name, child, modifierProperty, engine, structDef, enumNode);
}

bool SymbolTable::FindEnumByName(const char* name, const TypeDesc::EnumNode** outEnumNode, const TypeDesc** outEnumType) const
{
    int childCount = mChildren.Size();
    for (int i = 0; i < childCount; ++i)
    {
        if (mChildren[i]->FindEnumByName(name, outEnumNode, outEnumType))
        {
            return true;
        }
    }

    return mTypeTable.FindEnumByName(name, outEnumNode, outEnumType);
}

TypeDesc::EnumNode* SymbolTable::NewEnumNode()
{
    return mTypeTable.NewEnumNode();
}

FunDesc* SymbolTable::FindFunctionDescription(BlockScript::Ast::FunCall* functionCall)
{
    int childCount = mChildren.Size();
    for (int i = 0; i < childCount; ++i)
    {
        FunDesc* foundDesc = mChildren[i]->FindFunctionDescription(functionCall);
        if (foundDesc != nullptr)
        {
            return foundDesc;
        }
    }
    return mFunTable.Find(functionCall);
}

FunDesc* SymbolTable::CreateFunctionDescription(BlockScript::Ast::StmtFunDec* funDec)
{
    return mFunTable.Insert(funDec);
}

StackFrameInfo* SymbolTable::CreateFrame()
{
    StackFrameInfo* frame = &mFrames.PushEmpty();
    frame->Initialize(mAllocator);
    return frame;
}

StackFrameInfo* SymbolTable::GetRootGlobalFrame()
{
    return &mFrames[0];
}
