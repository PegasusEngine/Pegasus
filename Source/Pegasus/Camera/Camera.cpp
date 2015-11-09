/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Camera.cpp
//! \author	Karolyn Boulanger
//! \date	01st September 2014
//! \brief	Camera object, storing the settings of one camera in the scene

#include "Pegasus/Camera/Camera.h"

namespace Pegasus {
namespace Camera {


BEGIN_IMPLEMENT_PROPERTIES(Camera)
    IMPLEMENT_PROPERTY(Camera, Position)
END_IMPLEMENT_PROPERTIES(Camera)

//----------------------------------------------------------------------------------------

Camera::Camera()
{
    BEGIN_INIT_PROPERTIES(Camera)
        INIT_PROPERTY(Position)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

Camera::Camera(const Camera & camera)
{
    //! \todo IMPORTANT Implement the copy constructor for the property grid.
    //!       When a Camera is copied for example, the property grid is copied, but not the members.
    //!       They should be.
}

//----------------------------------------------------------------------------------------

Camera & Camera::operator=(const Camera & camera)
{
    if (&camera != this)
    {
        /****IMPLEMENT**/
    }
    return *this;
}

//----------------------------------------------------------------------------------------

Camera::~Camera()
{
}


}   // namespace Camera
}   // namespace Pegasus
