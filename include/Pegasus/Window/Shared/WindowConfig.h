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

#include <Pegasus/Core/Shared/OsDefs.h>

// Forward declarations
namespace Pegasus {
    namespace App {
        class Application;
    }
	namespace Alloc {
		class IAllocator;
	}
    namespace Core {
        class IApplicationContext;
    }
	namespace Render {
		class IDevice;
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
    Os::WindowHandle mParentWindowHandle;

    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this window
    Alloc::IAllocator* mRenderAllocator; //!< Allocator to use when creating this window's render resources
    Render::IDevice * mDevice; //!< Pointer to device
    Core::IApplicationContext* mWindowContext; //!< Context for this window
    int mWidth; //!< Initial width of the window in pixels (> 0)
    int mHeight; //!< Initial height of the window in pixels (> 0)
    bool mCreateVisible; //!< Whether to create the window as initially visible or not

#if PEGASUS_ENABLE_PROXIES

    //! Callback to the pegasus editor for redrawing the frame.
    //! This callback is to be used to request another frame within a window. This will gaurantee to delegate a new redrawing
    //! within the editors architecture. In the case of qt, redrawing must be forwarded all the way up to the ui thread. Having
    //! calls internally to UpdateWindow or RenderWindow won't work unless using this.
    typedef void(*RedrawProxyFun)(void*);
    void* mRedrawCallbackArg;
    RedrawProxyFun mRedrawEditorCallback;
#endif


    //! Constructor
    inline WindowConfig()
        :   mIsChild(false),
            mParentWindowHandle(0),
            mAllocator(nullptr),
            mRenderAllocator(nullptr),
            mDevice(nullptr),
            mWindowContext(nullptr),
            mWidth(960),
            mHeight(540),
            mCreateVisible(false)
#if PEGASUS_ENABLE_PROXIES
            ,mRedrawCallbackArg(nullptr),
            mRedrawEditorCallback(nullptr)
#endif

        { }
};

enum Keys
{
    KEYS_INVALID,
    KEYS_SHIFT,
    KEYS_ALT,
    KEYS_COUNT
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOWCONFIG_H
