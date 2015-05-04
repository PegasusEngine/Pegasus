/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraManager.cpp
//! \author	Kevin Boulanger
//! \date	04th September 2014
//! \brief	Camera manager, storing the global cameras

#include "Pegasus/Camera/CameraManager.h"

namespace Pegasus {
namespace Camera {


CameraManager::CameraManager(Alloc::IAllocator * allocator)
:   mAllocator(allocator)
{
}

//----------------------------------------------------------------------------------------

CameraManager::~CameraManager()
{
}

//----------------------------------------------------------------------------------------

Camera * CameraManager::GetCamera(CameraType cameraType)
{
    if (cameraType < CAMERATYPE_COUNT)
    {
        return &mCamera[cameraType];
    }
    else
    {
        PG_FAILSTR("Trying to get an invalid camera type (%u). The type should be less than %u.", cameraType, CAMERATYPE_COUNT);
        return nullptr;
    }
}


}   // namespace Camera
}   // namespace Pegasus
