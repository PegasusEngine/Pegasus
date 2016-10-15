/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowMessageHandler.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Message handling interface for platform-specific windows to use.

#ifndef PEGASUS_WND_IWINDOWMESSAGEHANDLER_H
#define PEGASUS_WND_IWINDOWMESSAGEHANDLER_H

#include "Pegasus/Core/Shared/OsDefs.h"
#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Window/IWindowComponent.h"
#endif

namespace Pegasus {
namespace Wnd {


//! Message handling interface for platform-impls
class IWindowMessageHandler
{
public:
    //! Destructor
    virtual ~IWindowMessageHandler() {}


    //! Handles the create message
    virtual void OnCreate(Os::WindowHandle handle) = 0;

    //! Handles the destroy message
    virtual void OnDestroy() = 0;

    //! Handles the repaint message
    virtual void OnRepaint() = 0;

#if PEGASUS_ENABLE_PROXIES
    //! Sends a message to the editor, so it can repaint the entire window.
    virtual void RequestRepaintEditorWindow() = 0;
#endif

    //! Handles the resize message
    //! \param width New width.
    //! \param height Nww height.
    virtual void OnResize(unsigned int width, unsigned int height) = 0;

#if PEGASUS_ENABLE_PROXIES
    //! Handles events on mouse clicks and such.
    virtual void OnMouseEvent(IWindowComponent::MouseButton button, bool isDown, float x, float y) = 0;

    //! Handles events on key presses
    virtual void OnKeyEvent(Pegasus::Wnd::Keys key, bool isDown) = 0;
#endif
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_IWINDOWMESSAGEHANDLER_H
