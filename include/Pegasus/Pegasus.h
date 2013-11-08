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

// Compilation macro definitions, assertion and log management
#include "Pegasus/PegasusInternal.h"

// Memory framework
#include "Pegasus/Memory/MemoryManager.h"

// Render context
#include "Pegasus/Render/RenderContext.h"

// Window base class
#include "Pegasus/Window/Window.h"

// Application base class
#include "Pegasus/Application/Application.h"

// Versioning
#include "Pegasus/Version.h"


#endif  // PEGASUS_PEGASUS_H
