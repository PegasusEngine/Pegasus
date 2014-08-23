/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Version.h
//! \author David Worsham
//! \date   05th October 2013
//! \brief  Methods for checking the version of the engine.

#ifndef PEGASUS_VERSION_H
#define PEGASUS_VERSION_H

namespace Pegasus
{
//! Description container of the pegasus engine description.
struct PegasusDesc
{
    //! Graphics API options
    enum GapiType
    {
        DIRECT_3D,
        OPEN_GL,
        OPEN_GLES
    };

    //! Pegasus Major version number
    int      mPegasusVersionMajor;

    //! Pegasus Minor version number
    int      mPegasusVersionMinor;

    //! Pegasus Graphics API Type
    GapiType mGapiType;

    //! Pegasus Graphics API Version
    int      mGapiVersion;

    //! null terminated string list of developers (no particular order)
    const char * const * mPegasusCredits;
};
}

#endif  // PEGASUS_VERSION_H
