/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Camera.h
//! \author	Kevin Boulanger
//! \date	01st September 2014
//! \brief	Camera object, storing the settings of one camera in the scene

#ifndef PEGASUS_CAMERA_CAMERA_H
#define PEGASUS_CAMERA_CAMERA_H

namespace Pegasus {
namespace Camera {


//! Camera object, storing the settings of one camera in the scene
class Camera
{
public:

    //! Constructor
    Camera();

    //! Destructor
    virtual ~Camera();

    //! \todo Add copy and = operators
};


}   // namespace Camera
}   // namespace Pegasus

#endif  // PEGASUS_CAMERA_CAMERA_H
