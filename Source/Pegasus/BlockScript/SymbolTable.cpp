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

TypeDesc* SymbolTable::InternalCreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    TypeDesc* child,
    TypeDesc::ModifierProperty modifierProperty,
    TypeDesc::AluEngine engine,
    BlockScript::Ast::StmtStructDef* structDef,
    EnumNode* enumNode ,
    PropertyNode* propertyNode,
    ObjectPropertyAccessorCallback propertyCallback
    
)
{
    return mTypeTable.CreateType(modifier, name, child, modifierProperty, engine, structDef, enumNode, propertyNode, propertyCallback);
}

TypeDesc* SymbolTable::CreateScalarType(const char* name, TypeDesc::AluEngine aluEngine)
{
    return InternalCreateType(
        TypeDesc::M_SCALAR,
        name,
        nullptr, //no child
        TypeDesc::ModifierProperty(), //no modifier property
        aluEngine
    );
}

TypeDesc* SymbolTable::CreateVectorType(const char* name, TypeDesc* childType, int vectorSize, TypeDesc::AluEngine aluEngine)
{
    TypeDesc::ModifierProperty modProp;
    modProp.VectorSize = vectorSize;
    return InternalCreateType(
        TypeDesc::M_VECTOR,
        name,
        childType,
        modProp, 
        aluEngine
    );
}

TypeDesc* SymbolTable::CreateObjectType(const char* name, PropertyNode* propertyList, ObjectPropertyAccessorCallback propertyCallback)
{
    TypeDesc::ModifierProperty modProp;
    modProp.ArraySize = 0;
    return InternalCreateType(
        TypeDesc::M_REFERECE,
        name,
        nullptr, // no child
        modProp, //no modifier
        TypeDesc::E_NONE, //no alu engine
        nullptr, //no struct def
        nullptr, //no enum definition
        propertyList, 
        propertyCallback
    );
}

TypeDesc* SymbolTable::CreateEnumType(const char* name, EnumNode* enumNode)
{
    return InternalCreateType(
        TypeDesc::M_ENUM,
        name,
        nullptr, //no children
        TypeDesc::ModifierProperty(), //no modifier property
        TypeDesc::E_NONE, //no alu engine
        nullptr, //no struct definition
        enumNode
    );
}

TypeDesc* SymbolTable::CreateStructType(const char* name, Pegasus::BlockScript::Ast::StmtStructDef* def)
{
    return InternalCreateType(
        TypeDesc::M_STRUCT,
        name,
        nullptr, //no children
        TypeDesc::ModifierProperty(), //no modifier property
        TypeDesc::E_NONE, //no alu engine
        def //no struct definition
    );
}

TypeDesc* SymbolTable::CreateStarType()
{
    return InternalCreateType(
        TypeDesc::M_STAR,
        "*"
    );
}

TypeDesc* SymbolTable::CreateArrayType(const char* name, TypeDesc* childType, int count)
{
    TypeDesc::ModifierProperty modProp; 
    modProp.ArraySize = count;
    return InternalCreateType(
        TypeDesc::M_ARRAY,
        name,
        childType,
        modProp
    );
}

bool SymbolTable::FindEnumByName(const char* name, const EnumNode** outEnumNode, const TypeDesc** outEnumType) const
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

EnumNode* SymbolTable::NewEnumNode()
{
    return mTypeTable.NewEnumNode();
}

PropertyNode* SymbolTable::NewPropertyNode()
{
    return mTypeTable.NewPropertyNode();
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
