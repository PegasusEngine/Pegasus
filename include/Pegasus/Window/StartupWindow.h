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

//! Specialized window for internal startup use
class StartupWindow : public Window
{
public:
    //! Constructor
    //! \param config Config struct for this window.
    StartupWindow(const WindowConfig& config);

    //! Destructor
    ~StartupWindow();


    //! Factory function to create a new StartupWindow
    //! \param config Config struct to use for the new window.
    //! \return New window.
    static Window* Create(const WindowConfig& config);


    // Window API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Refresh();
};

} // end namespace Window
} // end namespace Pegasus

#endif  // PEGASUS_WINDOW_STARTUPWINDOW_H
