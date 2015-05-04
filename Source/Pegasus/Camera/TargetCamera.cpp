/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TargetCamera.cpp
//! \author	Kevin Boulanger
//! \date	11th November 2014
//! \brief	Camera object using target and up vectors to define its orientation

#include "Pegasus/Camera/TargetCamera.h"

namespace Pegasus {
namespace Camera {


BEGIN_IMPLEMENT_PROPERTIES(TargetCamera)
    IMPLEMENT_PROPERTY(Math::Vec3, Target, Math::Vec3(0.0f, 0.0f, -1.0f))
    IMPLEMENT_PROPERTY(Math::Vec3, Up, Math::Vec3(0.0f, 1.0f, 0.0f))
END_IMPLEMENT_PROPERTIES()

//----------------------------------------------------------------------------------------

TargetCamera::TargetCamera()
:   Camera()
{
}

//----------------------------------------------------------------------------------------

TargetCamera::TargetCamera(const TargetCamera & camera)
:   Camera(camera)
{
    //! \todo Make sure the property grid of the parent and this class are properly copied
}

//----------------------------------------------------------------------------------------

TargetCamera & TargetCamera::operator=(const TargetCamera & camera)
{
    if (&camera != this)
    {
        Camera::operator=(camera);

        //! \todo Make sure the property grid of the parent and this class are properly copied
    }
    return *this;
}

//----------------------------------------------------------------------------------------

TargetCamera::~TargetCamera()
{
}


}   // namespace Camera
}   // namespace Pegasus
