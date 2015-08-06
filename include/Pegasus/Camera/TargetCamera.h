/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TargetCamera.h
//! \author	Karolyn Boulanger
//! \date	11th November 2014
//! \brief	Camera object using target and up vectors to define its orientation

#ifndef PEGASUS_CAMERA_TARGETCAMERA_H
#define PEGASUS_CAMERA_TARGETCAMERA_H

#include "Pegasus/Camera/Camera.h"

namespace Pegasus {
namespace Camera {


//! Camera object using target and up vectors to define its orientation
class TargetCamera : public Camera
{
    BEGIN_DECLARE_PROPERTIES()

        //! Position of the camera target in world space, cannot be equal to Position
        DECLARE_PROPERTY(Math::Vec3, Target)

        //! Up vector (normalized), cannot be orthogonal to the direction vector
        DECLARE_PROPERTY(Math::Vec3, Up)

    END_DECLARE_PROPERTIES()

    //------------------------------------------------------------------------------------
        
public:

    //! Constructor
    TargetCamera();

    //! Copy constructor
    //! \param camera Camera to copy the parameters from
    TargetCamera(const TargetCamera & camera);

    //! Destructor
    virtual ~TargetCamera();

    //! Assignment operator
    //! \param camera Camera to copy the parameters from
    //! \return Updated current camera
    TargetCamera & operator=(const TargetCamera & camera);
};


}   // namespace Camera
}   // namespace Pegasus

#endif  // PEGASUS_CAMERA_TARGETCAMERA_H
