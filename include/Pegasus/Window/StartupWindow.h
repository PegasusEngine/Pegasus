/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   StartupWindow.h
//! \author David Worsham
//! \date   20th October 2013
//! \brief  Specialized window for the internal startup window.

#ifndef PEGASUS_WINDOW_STARTUPWINDOW_H
#define PEGASUS_WINDOW_STARTUPWINDOW_H

#include "Pegasus/Window/Window.h"

namespace Pegasus {
namespace Window {

//! \class This is a specialized window for the TestApp1 application main window.  Place
//         specialized rendering code here.
class StartupWindow : public Window
{
public:
    // Ctor / dtor
    StartupWindow(const WindowConfig& config);
    ~StartupWindow();

    // Factory API
    static Window* Create(const WindowConfig& config);

    // App-specific API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Refresh();
};

} // end namespace Window
} // end namespace Pegasus

#endif  // PEGASUS_WINDOW_STARTUPWINDOW_H
