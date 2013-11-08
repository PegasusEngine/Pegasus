/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowConfig.h
//! \author David Worsham
//! \date   06th October 2013
//! \brief  Config structure for a window.

#ifndef PEGASUS_WND_WINDOWCONFIG_H
#define PEGASUS_WND_WINDOWCONFIG_H

#include "Pegasus/Window/WindowDefs.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class Application;
    }
    namespace Wnd {
        class IWindowContext;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Wnd {

//! Configuration structure for a Pegasus window
struct WindowConfig
{
public:
    //! True if the window needs to be a child of a given parent window (mParentWindowHandle)
    //! \warning This variable exists to handle cases where 0 is a valid window handle for a given operating system
    bool mIsChild;

    //! Platform-specific window handle of the parent window.
    //! Defined only when mIsChild == true
    //! \warning Assuming that mParentWindowHandle == 0 results in a non-child window will become a problem
    //!          if an operating system considers 0 as a valid handle. mIsChild is used to define if a parent window is actually defined
    WindowHandle mParentWindowHandle;

    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this window
    Alloc::IAllocator* mRenderAllocator; //!< Allocator to use when creating this window's render resources
    ModuleHandle mModuleHandle; //!< Opaque application instance
    IWindowContext* mWindowContext; //!< Context for this window
    int mWidth; //!< Initial width of the window in pixels (> 0)
    int mHeight; //!< Initial height of the window in pixels (> 0)
    bool mCreateVisible; //!< Whether to create the window as initially visible or not
    bool mUseBasicContext; //!< Whether to use a basic or extended context for this window

    //! Constructor
    inline WindowConfig()
        :   mIsChild(false),
            mParentWindowHandle(0),
            mAllocator(nullptr),
            mRenderAllocator(nullptr),
            mModuleHandle(0),
            mWindowContext(nullptr),
            mWidth(960),
            mHeight(540),
            mCreateVisible(false),
            mUseBasicContext(false)
        { }
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOWCONFIG_H
