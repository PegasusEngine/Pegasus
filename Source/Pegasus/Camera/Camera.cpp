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
    IMPLEMENT_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 0.0f, 0.0f))
END_IMPLEMENT_PROPERTIES()

//----------------------------------------------------------------------------------------

Camera::Camera()
:   mPropertyGrid()
{
}

//----------------------------------------------------------------------------------------

Camera::Camera(const Camera & camera)
:   mPropertyGrid(camera.mPropertyGrid)
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
        //! \todo IMPORTANT Implement the assignment operator for the property grid.
        //!       When a Camera is copied for example, the property grid is copied, but not the members.
        //!       They should be.
        mPropertyGrid = camera.mPropertyGrid;
    }
    return *this;
}

//----------------------------------------------------------------------------------------

Camera::~Camera()
{
}


}   // namespace Camera
}   // namespace Pegasus
