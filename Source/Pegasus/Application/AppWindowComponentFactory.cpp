/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppWindowComponentFactory.cpp
//! \author Kleber Garcia
//! \date   9/12/2015
//! \brief  Master class that contains all the components of an application.

#include "Pegasus/Application/AppWindowComponentFactory.h"
#include "Pegasus/Application/Components/WorldComponent.h"
#include "Pegasus/Application/Components/EditorComponents.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Window/IWindowComponent.h"
#include "Pegasus/Utils/Memset.h"

using namespace Pegasus;
using namespace Pegasus::App;
using namespace Pegasus::Wnd;

AppWindowComponentFactory::AppWindowComponentFactory(Alloc::IAllocator* allocator)
: mAllocator(allocator)
{
    Utils::Memset32(&mComponentInstances, 0, sizeof(mComponentInstances));
    mComponentInstances[COMPONENT_WORLD] = PG_NEW(allocator, -1, "WorldComponent", Pegasus::Alloc::PG_MEM_PERM) WorldComponent(allocator);
#if PEGASUS_ENABLE_PROXIES
    mComponentInstances[COMPONENT_TEXTURE_VIEW] = PG_NEW(allocator, -1, "TextureViewComponent", Pegasus::Alloc::PG_MEM_PERM) TextureViewComponent(allocator);
    mComponentInstances[COMPONENT_GRID] = PG_NEW(allocator, -1, "GridComponent", Pegasus::Alloc::PG_MEM_PERM) GridComponent(allocator);
    #if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
        mComponentInstances[COMPONENT_DEBUG_CAMERA] = PG_NEW(allocator, -1, "DebugCameraComponent", Pegasus::Alloc::PG_MEM_PERM) CameraDebugComponent(allocator);
    #endif
    #if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN
        mComponentInstances[COMPONENT_TERRAIN3D] = PG_NEW(allocator, -1, "Terrain3dDebugComponent", Pegasus::Alloc::PG_MEM_PERM) Terrain3dDebugComponent(allocator);
    #endif
#endif
}

AppWindowComponentFactory::~AppWindowComponentFactory()
{
    for (int i = 0; i < static_cast<int>(COMPONENT_COUNT); ++i)
    {
        if (mComponentInstances[i] != nullptr)
        {
            PG_DELETE(mAllocator, mComponentInstances[i]);
        }
    }
}

void AppWindowComponentFactory::LoadAllComponents(Core::IApplicationContext* appContext)
{
    for (int i = 0; i < static_cast<int>(COMPONENT_COUNT); ++i)
    {
        if (mComponentInstances[i] != nullptr)
        {
            mComponentInstances[i]->Load(appContext);
        }
    }
}

void AppWindowComponentFactory::UnloadAllComponents(Core::IApplicationContext* appContext)
{
    for (int i = 0; i < static_cast<int>(COMPONENT_COUNT); ++i)
    {
        if (mComponentInstances[i] != nullptr)
        {
            mComponentInstances[i]->Unload(appContext);
        }
    }
}

void AppWindowComponentFactory::UpdateAllComponents(Core::IApplicationContext* appContext)
{
    for (int i = 0; i < static_cast<int>(COMPONENT_COUNT); ++i)
    {
        IWindowComponent* c = mComponentInstances[i];
        if (c != nullptr)
        {   
            c->Update(appContext);
        }
    }
}

void AppWindowComponentFactory::AttachComponentsToWindow(Wnd::Window* window, ComponentTypeFlags components)
{
    unsigned int componentId = 0;
    while (components != 0 && componentId < COMPONENT_COUNT)
    {
        bool useComponent = components & 0x1;
        if (useComponent)
        {
            IWindowComponent* c = mComponentInstances[componentId];
            if (c != nullptr)
            {
                window->AttachComponent(c);
            }
            else
            {
                PG_LOG('ERR_', "Component %d requested but not available.", componentId);
            }
        }
        components = static_cast<ComponentTypeFlags>(components >> 0x1);
        ++componentId;
    }
}
