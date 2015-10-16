/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppWindowManager.h
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Window manager for a Pegasus app.

#ifndef PEGASUS_APP_APPWINDOWMANAGER_H
#define PEGASUS_APP_APPWINDOWMANAGER_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Utils/Vector.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class AppWindowComponentFactory;
    }

    namespace Core {
        class IApplicationContext;
    }

    namespace Wnd {
        class Window;
        struct WindowConfig;
    }

}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {

//! Window manager for a Pegasus app
//! This class manages a set of window for a Pegasus app
//! It allows the registration and creation of windows of multiple,
//! user defined types to make it simple to extend the runtime.
class AppWindowManager
{
public:
    //! Constructor
    //! \param config Config to create this manager with.
    AppWindowManager(Alloc::IAllocator* allocator);

    //! Destructor
    //! \warning Unregister your classes and destroy your windows before you call this!
    ~AppWindowManager();

    //! Creates a window and adds it to this manager.
    //! \param className Class of window to create.
    //! \param config Config to use for the window.
    //! \return Created window.
    Wnd::Window* CreateNewWindow(const Wnd::WindowConfig& config, ComponentTypeFlags componentFlags);

    //! Destroys a window and removes it from this manager.
    //! \param window The window to destroy.
    void DestroyWindow(Wnd::Window* window);

    //! Loads all the components in the component pool.
    //! \param context - the application context
    void LoadAllComponents(Core::IApplicationContext* context);

    //! Update all the components in the component pool.
    //! \param context - the application context
    void UpdateAllComponents(Core::IApplicationContext* context);

    //! Unload all the components
    //! \param context - the application context
    void UnloadAllComponents(Core::IApplicationContext* context);

private:
    // No copies allowed
    PG_DISABLE_COPY(AppWindowManager);
    int mWorldWindowsCount;
    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this object
    Utils::Vector<Wnd::Window*> mContainer;

    AppWindowComponentFactory*                      mComponentFactory;       //!< Component factory of windows.
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_APPWINDOWMANAGER_H
