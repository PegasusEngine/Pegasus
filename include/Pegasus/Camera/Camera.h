/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Camera.h
//! \author	Karolyn Boulanger
//! \date	01st September 2014
//! \brief	Camera object, storing the settings of one camera in the scene

#ifndef PEGASUS_CAMERA_CAMERA_H
#define PEGASUS_CAMERA_CAMERA_H

#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus {
namespace Camera {


//! Camera object, storing the settings of one camera in the scene
class Camera : public PropertyGrid::PropertyGridObject
{
    BEGIN_DECLARE_PROPERTIES_BASE(Camera)

        //! Position of the camera in world space
        DECLARE_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 0.0f, 0.0f))

    END_DECLARE_PROPERTIES()

    //------------------------------------------------------------------------------------
        
public:

    //! Constructor
    Camera();

    //! Copy constructor
    //! \param camera Camera to copy the parameters from
    Camera(const Camera & camera);

    //! Destructor
    virtual ~Camera();

    //! Assignment operator
    //! \param camera Camera to copy the parameters from
    //! \return Updated current camera
    Camera & operator=(const Camera & camera);
};


}   // namespace Camera
}   // namespace Pegasus

#endif  // PEGASUS_CAMERA_CAMERA_H
