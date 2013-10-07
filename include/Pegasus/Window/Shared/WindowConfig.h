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
namespace Window {

//! \class Configuration structure for a Pegasus window.
//! \todo We need actual params here
struct WindowConfig
{
public:
    //! Opaque application instance
    ModuleHandle mModuleHandle;

    //! Startup window flag
    //! This indicates that the window is a dummy,
    //! for creating the initial render context
    bool mIsStartupWindow;
};


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_WINDOWCONFIG_H
