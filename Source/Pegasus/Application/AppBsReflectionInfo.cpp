/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppBsReflectionInfo.cpp
//! \author Kleber Garcia
//! \date   July 26th, 2015
//! \brief  This class registers libraries that are exposed to the editor for documentation purposes.
//!         Use this api to see the available functions in blockscript

#if PEGASUS_ENABLE_BS_REFLECTION_INFO

#include "Pegasus/Application/AppBsReflectionInfo.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/BlockScript/BlockScriptAST.h"

using namespace Pegasus;
using namespace Pegasus::App;

BsTypeInfo::BsTypeInfo(Alloc::IAllocator* alloc, const BlockScript::TypeDesc* typeDesc)
: mChildren(alloc), mProps(alloc), mEnums(alloc), mTypeDesc(typeDesc), mAlloc(alloc)
{
    if (mTypeDesc->GetModifier() == BlockScript::TypeDesc::M_STRUCT)
    {
        const BlockScript::Ast::StmtStructDef* structDef = mTypeDesc->GetStructDef(); 
        const BlockScript::Ast::ArgList* argList = structDef->GetArgList();
        while (argList != nullptr && argList->GetArgDec() != nullptr)
        {
            NameTypePair& ntp = mChildren.PushEmpty();
            ntp.mType = PG_NEW(alloc, -1, "Allocator", Alloc::PG_MEM_TEMP) BsTypeInfo(alloc, argList->GetArgDec()->GetType());
            ntp.mName = argList->GetArgDec()->GetVar();
            argList = argList->GetTail();
        }
    }
    else if (mTypeDesc->GetModifier() == BlockScript::TypeDesc::M_REFERECE)
    {
        const BlockScript::PropertyNode* pn = mTypeDesc->GetPropertyNode();
        while (pn != nullptr)
        {
            NameTypePair& ntp = mProps.PushEmpty();
            ntp.mType = PG_NEW(alloc, -1, "Allocator", Alloc::PG_MEM_TEMP) BsTypeInfo(alloc, pn->mType);
            ntp.mName = pn->mName;
            pn = pn->mNext;
        }
    }
    else if (mTypeDesc->GetModifier() == BlockScript::TypeDesc::M_ENUM)
    {
        const BlockScript::EnumNode* en = mTypeDesc->GetEnumNode();
        while (en != nullptr)
        {
            mEnums.PushEmpty() = en->mIdd;
            en = en->mNext;
        }
    }
    
}

BsTypeInfo::~BsTypeInfo()
{
    if (mTypeDesc->GetModifier() == BlockScript::TypeDesc::M_STRUCT)
    {
        for (unsigned int i = 0; i < mChildren.GetSize(); ++i)
        {
            PG_DELETE(mAlloc, mChildren[i].mType);
        }
    }
    else if (mTypeDesc->GetModifier() == BlockScript::TypeDesc::M_REFERECE)
    {
        for (unsigned int i = 0; i < mProps.GetSize(); ++i)
        {
            PG_DELETE(mAlloc, mProps[i].mType);
        }
    }
}
    

const char* BsTypeInfo::GetName() const
{
    return mTypeDesc->GetName();
}

const char* BsTypeInfo::GetModifierName() const
{
    switch(mTypeDesc->GetModifier())
    {
    case BlockScript::TypeDesc::M_INVALID:
        return "Invalid";
    case BlockScript::TypeDesc::M_SCALAR :
        return "Scalar";
    case BlockScript::TypeDesc::M_VECTOR :
        return "Vector";
    case BlockScript::TypeDesc::M_ARRAY  :
        return "Array";
    case BlockScript::TypeDesc::M_STRUCT :
        return "Struct";
    case BlockScript::TypeDesc::M_ENUM   :
        return "Enum";
    case BlockScript::TypeDesc::M_REFERECE:
        return "ObjectRef";
    case BlockScript::TypeDesc::M_STAR    :
        return "*";
    default:
        break;
    }
    return "Unknown";
}

int BsTypeInfo::GetPropertiesCount() const
{
    return mProps.GetSize();
}

int BsTypeInfo::GetEnumsCount() const
{
    return mEnums.GetSize();
}

const NameTypePair*   BsTypeInfo::GetProperties() const
{
    return mProps.Data();
}

const char*const*   BsTypeInfo::GetEnums() const
{
    return mEnums.Data();
}

int BsTypeInfo::GetChildrenCount() const
{
    return mChildren.GetSize();
}

const NameTypePair* BsTypeInfo::GetChildren() const
{
    return mChildren.Data();
}

BsFunInfo::BsFunInfo(Alloc::IAllocator* alloc, const BlockScript::FunDesc* funDesc)
:
    mAlloc(alloc),
    mArgs(alloc),
    mFunDesc(funDesc),
    mReturnType(alloc, funDesc->GetDec()->GetReturnType())
{
    BlockScript::Ast::ArgList* argList = funDesc->GetDec()->GetArgList();
    while (argList != nullptr && argList->GetArgDec() != nullptr)
    {
        BlockScript::Ast::ArgDec* argDec = argList->GetArgDec();
        NameTypePair& p = mArgs.PushEmpty();
        p.mName = argDec->GetVar();
        p.mType = PG_NEW(alloc, -1, "Allocator", Alloc::PG_MEM_TEMP) BsTypeInfo(alloc, argDec->GetType());
        argList = argList->GetTail();
    }
}

BsFunInfo::~BsFunInfo()
{
    for (unsigned int i = 0; i < mArgs.GetSize(); ++i)
    {
        NameTypePair& p = mArgs[i];
        PG_DELETE(mAlloc, p.mType);
    }
}

const char* BsFunInfo::GetName() const
{
    return mFunDesc->GetDec()->GetName();
}

const IBsTypeInfo* BsFunInfo::GetReturnType() const
{
    return &mReturnType;
}

int BsFunInfo::GetArgCount() const
{
    return mArgs.GetSize();
}

const NameTypePair* BsFunInfo::GetArgs() const
{
    return mArgs.Data();
}

AppBlockscriptLibInfo::AppBlockscriptLibInfo(Alloc::IAllocator* allocator, const BlockScript::BlockLib* lib)
    : mAlloc(allocator), mLib(lib), mTypes(allocator), mFuns(allocator)
{
    const BlockScript::SymbolTable* st = mLib->GetSymbolTable();
    const BlockScript::TypeTable* tt = st->GetTypeTable();
    for (int i = 0; i < tt->GetTypeCount(); ++i)
    {
        const BlockScript::TypeDesc* td = tt->GetTypeByIndex(i);
        mTypes.PushEmpty() = PG_NEW(allocator, -1, "Allocator", Alloc::PG_MEM_TEMP) BsTypeInfo(allocator, td);
    }

    const BlockScript::FunTable* ft = st->GetFunTable();
    for (int i = 0; i < ft->GetSize(); ++i)
    {
        const BlockScript::FunDesc* fd = ft->GetDesc(i);
        mFuns.PushEmpty() = PG_NEW(allocator, -1, "Allocator", Alloc::PG_MEM_TEMP) BsFunInfo(allocator, fd);
    }
    
}

AppBlockscriptLibInfo::~AppBlockscriptLibInfo()
{
    for (unsigned int i = 0; i < mTypes.GetSize(); ++i)
    {
        PG_DELETE(mAlloc, mTypes[i]);
    }

    for (unsigned int i = 0; i < mFuns.GetSize(); ++i)
    {
        PG_DELETE(mAlloc, mFuns[i]);
    }
}

const char* AppBlockscriptLibInfo::GetName() const
{
    return mLib->GetName();
}

int AppBlockscriptLibInfo::GetTypeCounts() const
{
    return mTypes.GetSize();
}

IBsTypeInfo*const* AppBlockscriptLibInfo::GetTypes() const
{
    return mTypes.Data();
}

int AppBlockscriptLibInfo::GetFunctionCounts() const
{
    return mFuns.GetSize();
}

IBsFunInfo*const* AppBlockscriptLibInfo::GetFuns() const
{
    return mFuns.Data();
}


//! Constructor
AppBsReflectionInfo::AppBsReflectionInfo(Alloc::IAllocator* allocator)
: mAlloc(allocator)
{
}

//! Destructor
AppBsReflectionInfo::~AppBsReflectionInfo()
{
    for (unsigned int i = 0; i < mLibs.GetSize(); ++i)
    {
        PG_DELETE(mAlloc, mLibs[i]);
    }
}

//! \return the count of libraries
int AppBsReflectionInfo::GetLibraryCounts() const
{
    return mLibs.GetSize();
}

//! \return an array of library pointers
IAppBlockscriptLibInfo*const* AppBsReflectionInfo::GetLibs() const
{
    return mLibs.Data();
}

//! Register this blockscript reflection
void AppBsReflectionInfo::RegisterLib(const BlockScript::BlockLib* lib)
{
    mLibs.PushEmpty() = PG_NEW(mAlloc, -1, "Allocator", Alloc::PG_MEM_TEMP) AppBlockscriptLibInfo(mAlloc, lib);
}



#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
