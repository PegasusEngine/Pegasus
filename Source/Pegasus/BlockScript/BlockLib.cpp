/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockLib.cpp
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Represents a module with libraries, types and functions

#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/Allocator/IAllocator.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

 //! Constructor in case you did not notice
BlockLib::BlockLib(Alloc::IAllocator* allocator)
    : BlockScriptCompiler(allocator), mAllocator(allocator)
{
}

//!  Destructor
BlockLib::~BlockLib()
{
}

void BlockLib::CreateIntrinsicFunctions (const FunctionDeclarationDesc* descriptionList, int count)
{
    InternalCreateIntrinsicFunctions(descriptionList, count, /*no methods*/false);
}

void BlockLib::InternalCreateIntrinsicFunctions (const FunctionDeclarationDesc* descriptionList, int count, bool isMethods)
{
    BlockScriptBuilder* builder = GetBuilder();
    for (int i = 0; i < count; ++i)
    {
        const FunctionDeclarationDesc& desc = descriptionList[i];
        int argCount = -1;
        while (desc.argumentTypes[++argCount] != nullptr);
        builder->CreateIntrinsicFunction(
            desc.functionName,
            desc.argumentTypes,
            desc.argumentNames,
            argCount,
            desc.returnType,
            desc.callback,
            isMethods
        );
    }
}

void BlockLib::CreateEnumTypes(const EnumDeclarationDesc* descriptionList, int count)
{
    SymbolTable* st = GetSymbolTable();
    BlockScriptBuilder* builder = GetBuilder();

    for (int i = 0; i < count; ++i)
    {
        const EnumDeclarationDesc& def = descriptionList[i];
        TypeDesc::EnumNode* enumNode = st->NewEnumNode();
        TypeDesc::EnumNode* lastEl = enumNode;

        for (int enVal = 0; enVal < def.count; ++enVal)
        {
            lastEl->mIdd = builder->CopyString(def.enumList[enVal].enumName);
            lastEl->mGuid = def.enumList[enVal].enumVal;
            if (enVal != def.count - 1)
            {
                lastEl->mNext = st->NewEnumNode();
                lastEl = lastEl->mNext;
            }
        }

        st->CreateType(
            TypeDesc::M_ENUM,
            builder->CopyString(def.typeName),
            nullptr, //no child
            0, //no modifier property
            TypeDesc::E_NONE, //no ALU engine
            nullptr, //no struct def
            enumNode
        );
    }
}

void BlockLib::CreateStructTypes(const StructDeclarationDesc* descriptionList, int count)
{
    SymbolTable* st = GetSymbolTable();
    BlockScriptBuilder* builder = GetBuilder();
    for (int i = 0; i < count; ++i)
    {
        const StructDeclarationDesc& desc = descriptionList[i];

        builder->StartNewFrame();
        
        //building the link list of types
        int c = 0;
        const char* currType = desc.memberTypes[c];
        Ast::ArgList* argList = builder->CreateArgList();
        Ast::ArgList* currArgList = argList;
        while (currType != nullptr)
        {
            const TypeDesc* typeDesc = st->GetTypeByName(currType);
            PG_ASSERTSTR(typeDesc != nullptr, "Type cannot be null! check that the type %s has been registered", currType);
            Ast::ArgDec* argDec = builder->BuildArgDec(builder->CopyString(desc.memberNames[c]), typeDesc);

            currArgList->SetArgDec(argDec);
            currType = desc.memberTypes[++c];

            if (currType != nullptr)
            {
                currArgList->SetTail(builder->CreateArgList());
                currArgList = currArgList->GetTail();
            }
        }

        Ast::StmtStructDef* def = builder->BuildStmtStructDef(builder->CopyString(desc.structTypeName), argList);
        PG_ASSERTSTR(def != nullptr, "FATAL! could not register the structure definition %s.", desc.structTypeName);
    }
}

void BlockLib::CreateClassTypes(const ClassTypeDesc* descriptionList, int count)
{
    SymbolTable* st = GetSymbolTable();
    BlockScriptBuilder* builder = GetBuilder();
    for (int i = 0; i < count; ++i)
    {
        const ClassTypeDesc& desc = descriptionList[i];

        st->CreateType(
            TypeDesc::M_REFERECE,
            desc.classTypeName,
            nullptr, //no child
            0, //no modifier property yet...
            TypeDesc::E_NONE, //no engine
            nullptr, //no struct definition
            nullptr // no enumeration mode
        );

        InternalCreateIntrinsicFunctions(desc.methodDescriptors, desc.methodsCount, /*is a method*/ true);
            
    }
}

SymbolTable* BlockLib::GetSymbolTable()
{
    return mBuilder.GetSymbolTable();
}
