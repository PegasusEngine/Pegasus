/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptManager.cpp
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Blockscript manager class. Use this as main entry point for blockscript creation

#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"

using namespace Pegasus::Alloc;
using namespace Pegasus::BlockScript;

BlockScriptManager::BlockScriptManager(IAllocator* allocator)
: mAllocator(nullptr), mInternalRuntimeLib(nullptr)
{
    Initialize(allocator);
}

BlockScriptManager::~BlockScriptManager()
{
    if (mInternalRuntimeLib != nullptr)
    {
        PG_DELETE(mAllocator, mInternalRuntimeLib);
    }
}

void BlockScriptManager::Initialize(IAllocator* allocator)
{
    mAllocator = allocator;
    mInternalRuntimeLib = CreateBlockLib();
    RegisterIntrinsics(mInternalRuntimeLib);
}

BlockScript* BlockScriptManager::CreateBlockScript()
{
    PG_ASSERTSTR(mInternalRuntimeLib != nullptr, "Internal runtime library cannot be null");
    return PG_NEW(mAllocator, -1, "Block Script", Alloc::PG_MEM_PERM) BlockScript(mAllocator, mInternalRuntimeLib);
}

BlockLib*    BlockScriptManager::CreateBlockLib()
{
    return PG_NEW(mAllocator, -1, "Block Script Lib Module", Alloc::PG_MEM_PERM) BlockLib(mAllocator);
}

BlockLib*    BlockScriptManager::GetRuntimeLib()
{
    return mInternalRuntimeLib;
}

void BlockScriptManager::DestroyBlockScript(BlockScript* script)
{
    PG_DELETE(mAllocator, script);
}

void BlockScriptManager::DestroyBlockLib(BlockLib* lib)
{
    PG_DELETE(mAllocator, lib);
}
