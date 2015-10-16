/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppWindowManager.cpp
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Window manager for a Pegasus app.

#include "Pegasus/Application/AppWindowManager.h"
#include "Pegasus/Application/AppWindowComponentFactory.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Window/Window.h"

namespace Pegasus {
namespace App {


//----------------------------------------------------------------------------------------

AppWindowManager::AppWindowManager(Alloc::IAllocator* allocator)
    : mAllocator(allocator), mContainer(allocator), mWorldWindowsCount(0)
{

    mComponentFactory = PG_NEW(mAllocator, -1, "ComponentFactory", Alloc::PG_MEM_PERM) AppWindowComponentFactory(mAllocator);
}

//----------------------------------------------------------------------------------------

AppWindowManager::~AppWindowManager()
{
    PG_DELETE(mAllocator, mComponentFactory);
}

//----------------------------------------------------------------------------------------

void AppWindowManager::LoadAllComponents(Core::IApplicationContext* context)
{
    mComponentFactory->LoadAllComponents(context);
}

//----------------------------------------------------------------------------------------

void AppWindowManager::UpdateAllComponents(Core::IApplicationContext* context)
{
    mComponentFactory->UpdateAllComponents(context);
}

//----------------------------------------------------------------------------------------

void AppWindowManager::UnloadAllComponents(Core::IApplicationContext* context)
{
    mComponentFactory->UnloadAllComponents(context);
}

//----------------------------------------------------------------------------------------

Wnd::Window* AppWindowManager::CreateNewWindow(const Wnd::WindowConfig& config, ComponentTypeFlags componentFlags)
{

    Wnd::Window* wnd  = PG_NEW(mAllocator, -1, "New Window", Alloc::PG_MEM_TEMP ) Wnd::Window(config);
    mComponentFactory->AttachComponentsToWindow(wnd, componentFlags);

    if (componentFlags & COMPONENT_FLAG_WORLD && !config.mIsChild )
    {
        if (mWorldWindowsCount == 0) // give the first world window the ownership of the app
        {
            wnd->HandleMainWindowEvents();
        }
        ++mWorldWindowsCount;
    }
    mContainer.PushEmpty() = wnd;

    return wnd;
}

//----------------------------------------------------------------------------------------

void AppWindowManager::DestroyWindow(Wnd::Window* window)
{
    for (int i = 0; i < mContainer.GetSize(); ++i)
    {
        if (mContainer[i] == window)
        {
            PG_DELETE(mAllocator, window);
            mContainer.Delete(i);
            return;
        }
    }

    PG_FAILSTR("Could not detach window that does not belong to this application.");
}

}   // namespace App
}   // namespace Pegasus
