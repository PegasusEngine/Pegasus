/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TargetCamera.cpp
//! \author	Karolyn Boulanger
//! \date	11th November 2014
//! \brief	Camera object using target and up vectors to define its orientation

#include "Pegasus/Camera/TargetCamera.h"

namespace Pegasus {
namespace Camera {


BEGIN_IMPLEMENT_PROPERTIES(TargetCamera)
    IMPLEMENT_PROPERTY(TargetCamera, Target)
    IMPLEMENT_PROPERTY(TargetCamera, Up)
END_IMPLEMENT_PROPERTIES(TargetCamera)

//----------------------------------------------------------------------------------------

TargetCamera::TargetCamera()
:   Camera()
{
    BEGIN_INIT_PROPERTIES(TargetCamera)
        INIT_PROPERTY(Target)
        INIT_PROPERTY(Up)
    END_INIT_PROPERTIES()
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
