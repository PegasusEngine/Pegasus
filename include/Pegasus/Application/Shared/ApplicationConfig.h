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

//! \class Shared configuration structure for a Pegasus app.
struct ApplicationConfig
{
public:
    //! Handle to the module containing this application
    Window::ModuleHandle mModuleHandle;

    //! Default constructor
    inline ApplicationConfig() : mModuleHandle(0) { }
};

//! \class Shared configuration structure for a Pegasus application window.
struct AppWindowConfig
{
public:
    //! True if the window needs to be a child of a given parent window (mParentWindowHandle)
    bool mIsChild;

    //! Platform-specific window handle of the parent window.
    //! Defined only when mIsChild == true
    Window::WindowHandle mParentWindowHandle;

    //! Initial width of the window in pixels (> 0)
    int mWidth;

    //! Initial height of the window in pixels (> 0)
    int mHeight;

    //! Default constructor
    inline AppWindowConfig()
        :   mIsChild(false),
            mParentWindowHandle(0),
            mWidth(960),
            mHeight(540)
        { }
};


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
