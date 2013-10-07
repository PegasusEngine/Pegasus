/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowProxy.cpp
//! \author David Worsham
//! \date   06th October 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app window.

#include "Pegasus\Window\WindowProxy.h"
#include "Pegasus\Window\Window.h"

namespace Pegasus {
namespace Window {

WindowProxy::WindowProxy(Window* wnd)
    : mObject(wnd)
{
}

//----------------------------------------------------------------------------------------

WindowProxy::~WindowProxy()
{
}

//----------------------------------------------------------------------------------------

Window* WindowProxy::Unwrap() const
{
    return mObject;
}

//----------------------------------------------------------------------------------------


}   // namespace Window
}   // namespace Pegasus
