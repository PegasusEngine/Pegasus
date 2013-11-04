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

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus\Application\ApplicationProxy.h"

#ifdef __cplusplus
extern "C" {
#endif
PEGASUSAPP_SHARED Pegasus::Application::IApplicationProxy* CreatePegasusApp(const Pegasus::Application::ApplicationConfig& config)
{
    Pegasus::Memory::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    return PG_NEW(globalAlloc, "ApplicationProxy", Pegasus::Memory::PG_MEM_PERM) Pegasus::Application::ApplicationProxy(config);
}

PEGASUSAPP_SHARED void DestroyPegasusApp(Pegasus::Application::IApplicationProxy* app)
{
    Pegasus::Memory::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    PG_DELETE(globalAlloc, app);
}
#ifdef __cplusplus
}
#endif

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_APPEXPORT_H
