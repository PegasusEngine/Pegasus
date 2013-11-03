/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowMessageHandler.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Message handling interface for platform-specific windows to use.

#ifndef PEGASUS_WINDOW_IWINDOWMESSAGEHANDLER_H
#define PEGASUS_WINDOW_IWINDOWMESSAGEHANDLER_H

#include "Pegasus/Render/RenderDefs.h"

namespace Pegasus {
namespace Window {


//! Message handling interface for platform-impls
class IWindowMessageHandler
{
public:
    //! Destructor
    virtual ~IWindowMessageHandler() {}


    //! Handles the create message
    virtual void OnCreate(Render::DeviceContextHandle handle) = 0;

    //! Handles the destroy message
    virtual void OnDestroy() = 0;

    //! Handles the repaint message
    virtual void OnRepaint() = 0;

    //! Handles the resize message
    //! \param width New width.
    //! \param height Nww height.
    virtual void OnResize(unsigned int width, unsigned int height) = 0;
};


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_IWINDOWMESSAGEHANDLER_H
