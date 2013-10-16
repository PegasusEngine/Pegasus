/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PegasusInternal.h
//! \author Kevin Boulanger
//! \date   07th October 2013
//! \brief  Global include file for the Pegasus engine, included by any file from the Pegasus library

#ifndef PEGASUS_PEGASUSINTERNAL_H
#define PEGASUS_PEGASUSINTERNAL_H


// Compilation macro definitions
// (included by the project when compiling engine, but required when compiling an app)
#include "Pegasus/Preprocessor.h"

// Pegasus memory management
#include "Pegasus/Memory/Memory.h"

// Assertion management, can be used anywhere
#include "Pegasus/Core/Assertion.h"

// Log messages, can be used anywhere
#include "Pegasus/Core/Log.h"


#endif  // PEGASUS_PEGASUSINTERNAL_H
