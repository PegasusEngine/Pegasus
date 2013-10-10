/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowConfig.h
//! \author David Worsham
//! \date   6th October 2013
//! \brief  Config structure for a window.

#ifndef PEGASUS_WINDOW_WINDOWCONFIG_H
#define PEGASUS_WINDOW_WINDOWCONFIG_H

#include "Pegasus/Window/WindowDefs.h"

namespace Pegasus {
    namespace Application {
        class Application;
    }
}

namespace Pegasus {
namespace Window {


//! \class Configuration structure for a Pegasus window.
//! \todo We need actual params here
struct WindowConfig
{
public:
    //! Opaque application instance
    ModuleHandle mModuleHandle;

    //! Application for which the window will belong to
    Application::Application * mApplication;

    //! Startup window flag
    //! This indicates that the window is a dummy,
    //! for creating the initial render context
    bool mIsStartupWindow;

    //! True if the window needs to be a child of a given parent window (mParentWindowHandle)
    //! \warning This variable exists to handle cases where 0 is a valid window handle for a given operating system
    bool mIsChild;

    //! Platform-specific window handle of the parent window.
    //! Defined only when mIsChild == true
    //! \warning Assuming that mParentWindowHandle == 0 results in a non-child window will become a problem
    //!          if an operating system considers 0 as a valid handle. mIsChild is used to define if a parent window is actually defined
    WindowHandle mParentWindowHandle;

    //! Initial width of the window in pixels (> 0)
    int mWidth;

    //! Initial height of the window in pixels (> 0)
    int mHeight;

    //! Default constructor
    inline WindowConfig()
        :   mModuleHandle(0),
            mApplication(nullptr),
            mIsStartupWindow(false),
            mIsChild(false),
            mWidth(960),
            mHeight(540),
            mParentWindowHandle(0)
        { }
};


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_WINDOWCONFIG_H
