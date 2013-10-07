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
};

//! \class Shared configuration structure for a Pegasus application window.
struct AppWindowConfig
{
public:
};


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
