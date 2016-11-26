/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderSystemManager.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  Main app entry point to manage systems. Use this class to register custom systems from the app

#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/RenderSystems/Grass/GrassSystem.h"
#include "Pegasus/RenderSystems/2dTerrain/2dTerrainSystem.h"
#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"
#include "Pegasus/RenderSystems/DeferredRenderer/DeferredRendererSystem.h"
#include "Pegasus/RenderSystems/Camera/CameraSystem.h"
#include "Pegasus/Allocator/Alloc.h"

using namespace Pegasus;
using namespace Pegasus::RenderSystems;

int RenderSystems::RenderSystem::sNextId = 0;

RenderSystemManager::~RenderSystemManager()
{
    PG_ASSERTSTR(mSystems.GetSize() == 0, "WARNING: must destroy all systems before exiting.");
}

void RenderSystemManager::AddInternalSystems()
{
//a lot of render systems depend on camera, so registering it first.
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    RegisterSystem(mAllocator, PG_NEW(mAllocator, -1, "Deferred Renderer System", Alloc::PG_MEM_PERM) Camera::CameraSystem(mAllocator));
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_GRASS
    //RegisterSystem(mAllocator, PG_NEW(mAllocator, -1, "Grass System", Alloc::PG_MEM_PERM) GrassSystem(mAllocator));
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_2DTERRAIN
    //RegisterSystem(mAllocator, PG_NEW(mAllocator, -1, "2d Terrain System", Alloc::PG_MEM_PERM) 2dTerrainSystem(mAllocator));
#endif
    
#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN
    RegisterSystem(mAllocator, PG_NEW(mAllocator, -1, "3d Terrain System", Alloc::PG_MEM_PERM) Terrain3dSystem(mAllocator));
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_DEFERREDRENDERER
    RegisterSystem(mAllocator, PG_NEW(mAllocator, -1, "Deferred Renderer System", Alloc::PG_MEM_PERM) DeferredRendererSystem(mAllocator));
#endif
}

void RenderSystemManager::RegisterSystem(Alloc::IAllocator* alloc, RenderSystem* system)
{
    RenderSystemManager::RenderSystemContainer& container = mSystems.PushEmpty();
    container.hash = Utils::HashStr(system->GetSystemName());
    container.allocator = alloc;
    container.system = system;
    if (system->CanCreateBlockScriptApi())
    {
        container.blockscriptLib = mAppContext->GetBlockScriptManager()->CreateBlockLib(system->GetSystemName());
        mLibs.PushEmpty() = container.blockscriptLib;
    }
    else
    {
        container.blockscriptLib = nullptr;
    }
}

const RenderSystemManager::RenderSystemContainer* RenderSystemManager::FindSystem(const char* systemName)
{
    unsigned int hash = Utils::HashStr(systemName);
    for (unsigned int i = 0; i < mSystems.GetSize(); ++i)
    {
        if (mSystems[i].hash == hash)
        {
            return &mSystems[i];
        }
    }

    return nullptr;
}

void RenderSystemManager::InitializeSystems(Core::IApplicationContext* appContext)
{
    for (unsigned int i = 0; i < mSystems.GetSize(); ++i)
    {
        RenderSystemManager::RenderSystemContainer& container = mSystems[i];
        container.system->Load(appContext);
        if (container.blockscriptLib != nullptr)
        {
            //register the render api in case the render api types want to get used
            container.blockscriptLib->GetSymbolTable()->RegisterChild(appContext->GetRenderBsApi()->GetSymbolTable());

            container.system->OnRegisterBlockscriptApi(container.blockscriptLib, mAppContext);

            //unregister the render api, once compilation is done.
            // render api will be available through  the global list of libs, and this will avoid looking symbols into the render api twice.
            container.blockscriptLib->GetSymbolTable()->UnregisterChild(appContext->GetRenderBsApi()->GetSymbolTable());
        }

        container.system->OnRegisterCustomMeshNodes(mAppContext->GetMeshManager());
        container.system->OnRegisterCustomTextureNodes(mAppContext->GetTextureManager());
    }
}

void RenderSystemManager::DestroyAllSystems()
{
    for (unsigned int i = 0; i < mSystems.GetSize(); ++i)
    {
        RenderSystemManager::RenderSystemContainer& container = mSystems[i];
        if (container.blockscriptLib != nullptr)
        {
            mAppContext->GetBlockScriptManager()->DestroyBlockLib(container.blockscriptLib);
        }
        PG_DELETE(container.allocator, container.system);
    }

    mSystems.Clear();
}