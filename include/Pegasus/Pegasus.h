/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Pegasus.h
//! \author Kevin Boulanger
//! \date   12th June 2013
//! \brief  Main include file of the Pegasus engine, to be included by any app

#ifndef PEGASUS_PEGASUS_H
#define PEGASUS_PEGASUS_H

//== Includes ====

// Compilation macro definitions, assertion and log management
#include "Pegasus/PegasusInternal.h"

// Application base class
#include "Pegasus/Application/Application.h"

// Window base class
//#include "Pegasus/Window/Window.h"

// Exported Pegasus methods
#if PEGASUSAPP_DLL
#include "Pegasus/AppExport.h"
#endif
#include "Pegasus/Version.h"

#endif  // PEGASUS_PEGASUS_H
