/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Version.h
//! \author David Worsham
//! \date   5th October 2013
//! \brief  Methods for checking the version of the engine.

#ifndef PEGASUS_VERSION_H
#define PEGASUS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif
//! Get library version number
PEGASUSAPP_SHARED unsigned int GetPegasusVersion();

#ifdef __cplusplus
}
#endif

#endif  // PEGASUS_VERSION_H
