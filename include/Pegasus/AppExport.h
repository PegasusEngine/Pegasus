/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AppExport.h
//! \author David Worsham
//! \date   05th October 2013
//! \brief  Exported functions for creating an app proxy via the launcher or editor.

#ifndef PEGASUS_APPEXPORT_H
#define PEGASUS_APPEXPORT_H

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Application/ApplicationProxy.h"
#include "Pegasus/Memory/MemoryManager.h"

#ifdef __cplusplus
extern "C" {
#endif
PEGASUSAPP_SHARED Pegasus::App::IApplicationProxy* CreatePegasusApp(const Pegasus::App::ApplicationConfig& config)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    return PG_NEW(globalAlloc, -1, "ApplicationProxy", Pegasus::Alloc::PG_MEM_PERM) Pegasus::App::ApplicationProxy(config);
}

PEGASUSAPP_SHARED void DestroyPegasusApp(Pegasus::App::IApplicationProxy* app)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    PG_DELETE(globalAlloc, app);
}
#ifdef __cplusplus
}
#endif

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_APPEXPORT_H
