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
#include "Pegasus/Application/IWindowRegistry.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        struct TypeTable;
        struct WindowTable;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {

//! Config object for the window manager class
struct AppWindowManagerConfig
{
public:
    //! Constructor
    AppWindowManagerConfig() : mAllocator(nullptr), mMaxWindowTypes(2), mMaxNumWindows(1) {}

    //! Destructor
    ~AppWindowManagerConfig() {};


    Alloc::IAllocator* mAllocator; //!< Allocator used to create this object
    unsigned int mMaxWindowTypes; //!< Maximum number of window types this manager can contain
    unsigned int mMaxNumWindows; //!< Maximum number of windows this manager can contain
};


//! Window manager for a Pegasus app
//! This class manages a set of window for a Pegasus app
//! It allows the registration and creation of windows of multiple,
//! user defined types to make it simple to extend the runtime.
class AppWindowManager : public IWindowRegistry
{
public:
    //! Constructor
    //! \param config Config to create this manager with.
    AppWindowManager(const AppWindowManagerConfig& config);

    //! Destructor
    //! \warning Unregister your classes and destroy your windows before you call this!
    ~AppWindowManager();


    // IWindowRegistry API
    virtual void RegisterWindowClass(const char* className, const WindowRegistration& classReg);
    virtual void UnregisterWindowClass(const char* className);
    virtual const char* GetMainWindowType() const;
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    virtual const char* GetSecondaryWindowType() const;
#endif

    //! Creates a window and adds it to this manager.
    //! \param className Class of window to create.
    //! \param config Config to use for the window.
    //! \return Created window.
    Wnd::Window* CreateWindow(const char* className, const Wnd::WindowConfig& config);

    //! Destroys a window and removes it from this manager.
    //! \param window The window to destroy.
    void DestroyWindow(Wnd::Window* window);


    static const unsigned int APPWINDOW_DESC_LENGTH = 128; //! Max length of an app window description
    static const unsigned int APPWINDOW_CLASS_LENGTH = 32; //! Max length of an app window type

private:
    // No copies allowed
    PG_DISABLE_COPY(AppWindowManager);


    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this object
    TypeTable* mTypeTable; //!< Window type table
    WindowTable* mWindowTable; //!< Window table
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_APP_APPWINDOWMANAGER_H
