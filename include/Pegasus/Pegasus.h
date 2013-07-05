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
// Compilation macro definitions
// (included by the project when compiling engine, but required when compiling an app)
#include "Pegasus/Preprocessor.h"
#include "Pegasus/PegasusApp.h" // App skeleton
#include "Pegasus/Core/Window/PegasusWindow.h" // Window skeleton

// Assertion management, can be used anywhere
#include "Pegasus/Core/Assertion.h"

// Log messages, can be used anywhere
#include "Pegasus/Core/Log.h"

//== Forward Declarations ====

//== Interface ====


#endif  // PEGASUS_PEGASUS_H
