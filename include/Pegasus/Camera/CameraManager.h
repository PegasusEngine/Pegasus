/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraManager.h
//! \author	Kevin Boulanger
//! \date	04th September 2014
//! \brief	Camera manager, storing the global cameras

#ifndef PEGASUS_CAMERA_CAMERAMANAGER_H
#define PEGASUS_CAMERA_CAMERAMANAGER_H

#include "Pegasus/Camera/Camera.h"

namespace Pegasus {
namespace Camera {


//! Camera manager, storing the global cameras
class CameraManager
{
public:

    //! Constructor
    //! \param allocator Allocator used for all camera allocations
    CameraManager(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~CameraManager();


    //! List of global cameras, that can be accessed and modified at any time
    enum CameraType
    {
        CAMERATYPE_MAIN = 0,        //!< Main camera, used for rendering the main scene

        CAMERATYPE_COUNT
    };

    //! Get one of the global cameras
    //! \param cameraType Type of the camera to get (CAMERATYPE_xxx constant)
    //! \return Global camera, nullptr in case of error
    Camera * GetCamera(CameraType cameraType);

    //------------------------------------------------------------------------------------

private:

    // The manager cannot be copied
    PG_DISABLE_COPY(CameraManager)

    //! Allocator used for all camera allocations
    Alloc::IAllocator * mAllocator;


    //! List of global cameras
    Camera mCamera[CAMERATYPE_COUNT];
};


}   // namespace Camera
}   // namespace Pegasus

#endif  // PEGASUS_CAMERA_CAMERAMANAGER_H
