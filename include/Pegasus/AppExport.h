/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppExport.h
//! \author David Worsham
//! \date   5th October 2013
//! \brief  Exported functions for creating an app proxy via the launcher or editor.

#ifndef PEGASUS_APPEXPORT_H
#define PEGASUS_APPEXPORT_H

#include "Pegasus\Application\ApplicationProxy.h"

#ifdef __cplusplus
extern "C" {
#endif
PEGASUSAPP_SHARED Pegasus::Application::IApplicationProxy* CreatePegasusApp()
{
    return new Pegasus::Application::ApplicationProxy();
}

PEGASUSAPP_SHARED void DestroyPegasusApp(Pegasus::Application::IApplicationProxy* app)
{
    delete app;
}
#ifdef __cplusplus
}
#endif

#endif  // PEGASUS_APPEXPORT_H
