/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Version.cpp
//! \author David Worsham
//! \date   05th October 2013
//! \brief  Methods for checking the version of the engine.

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Version.h"

// Pegasus Engine version
#define PEGASUS_ENGINE_VERSION_MAJOR 0
#define PEGASUS_ENGINE_VERSION_MINOR 2

// Pegasus Developers namelist
const char * const gCredits [] = {
    "Karolyn Boulanger",
    "Kleber Garcia",
    "David Worsham",
    nullptr
};

namespace Pegasus
{
    extern const Pegasus::PegasusAssetTypeDesc* const* GetAllAssetTypesDescs();
}

void Internal_GetEngineDesc(Pegasus::PegasusDesc& engineDesc)
{
#if PEGASUS_GAPI_GL
    engineDesc.mGapiType = Pegasus::PegasusDesc::OPEN_GL;
    engineDesc.mGapiVersion = 4;
#elif PEGASUS_GAPI_DX
    engineDesc.mGapiType = Pegasus::PegasusDesc::DIRECT_3D;
#if PEGASUS_GAPI_DX11
    engineDesc.mGapiVersion = 11; 
#elif PEGASUS_GAPI_DX12
    engineDesc.mGapiVersion = 12; 
#endif
#elif PEGASUS_GAPI_GLES
    #error Unsupported Pegasus GAPI GLES
#else
    #error Undefined GAPI
#endif

    engineDesc.mPegasusVersionMajor = PEGASUS_ENGINE_VERSION_MAJOR;
    engineDesc.mPegasusVersionMinor = PEGASUS_ENGINE_VERSION_MINOR;
    engineDesc.mPegasusCredits = gCredits;
    engineDesc.mAssetTypes = Pegasus::GetAllAssetTypesDescs();
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_ENABLE_PROXIES
