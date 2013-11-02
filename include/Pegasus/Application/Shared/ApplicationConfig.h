/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationConfig.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Shared config structure for a Pegasus application.

#ifndef PEGASUS_SHARED_APPLICATIONCONFIG_H
#define PEGASUS_SHARED_APPLICATIONCONFIG_H

#include "Pegasus/Window/WindowDefs.h"

namespace Pegasus {
namespace Application {

//! Shared configuration structure for a Pegasus app
struct ApplicationConfig
{
public:
    Window::ModuleHandle mModuleHandle; //!< Handle to the module containing this application
    unsigned int mMaxWindowTypes; //!< Maximum number of window types this app can contain
    unsigned int mMaxNumWindows; //!< Maximum number of windows this app can contain
    const char* mBasePath; //<! The base path to load all assets from

    // Debug API
#if PEGASUS_ENABLE_LOG
    Core::LogManager::Handler mLoghandler; //<! Log handler for this application
#endif
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::Handler mAssertHandler; //<! Assert handler for this application
#endif

    //! Default constructor
    inline ApplicationConfig()
        : mModuleHandle(0), mMaxWindowTypes(2), mMaxNumWindows(1), mBasePath(nullptr)
#if PEGASUS_ENABLE_LOG
          ,mLoghandler(nullptr)
#endif
#if PEGASUS_ENABLE_ASSERT
          ,mAssertHandler(nullptr)
#endif
    {}
};

//! \class Shared configuration structure for a Pegasus application window
struct AppWindowConfig
{
public:
    //! True if the window needs to be a child of a given parent window (mParentWindowHandle)
    //! \warning This variable exists to handle cases where 0 is a valid window handle for a given operating system
    bool mIsChild;

    //! Platform-specific window handle of the parent window.
    //! Defined only when mIsChild == true
    //! \warning Assuming that mParentWindowHandle == 0 results in a non-child window will become a problem
    //!          if an operating system considers 0 as a valid handle. mIsChild is used to define if a parent window is actually defined
    Window::WindowHandle mParentWindowHandle;

    const char* mWindowType; //!< Type string for the window
    int mWidth; //!< Initial width of the window in pixels (> 0)
    int mHeight; //!< Initial height of the window in pixels (> 0)

    //! Default constructor
    inline AppWindowConfig()
        :   mIsChild(false),
            mParentWindowHandle(0),
            mWindowType(nullptr),
            mWidth(960),
            mHeight(540)
        {}
};


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
