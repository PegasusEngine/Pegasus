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

#include "Pegasus/PropertyGrid/PropertyGrid.h"

namespace Pegasus {
namespace Camera {


//! Camera object, storing the settings of one camera in the scene
class Camera
{
    BEGIN_DECLARE_PROPERTIES()

        //! Position of the camera in world space
        DECLARE_PROPERTY(Math::Vec3, Position)

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


    //! Get the property grid
    //! \return Reference to the property grid
    inline const PropertyGrid::PropertyGrid & GetPropertyGrid() const { return mPropertyGrid; }

    //! Get the property grid
    //! \return Reference to the property grid
    inline PropertyGrid::PropertyGrid & GetPropertyGrid() { return mPropertyGrid; }

    //------------------------------------------------------------------------------------

private:

    //! Property grid, defining the set of editable properties
    PropertyGrid::PropertyGrid mPropertyGrid;
};


}   // namespace Camera
}   // namespace Pegasus

#endif  // PEGASUS_CAMERA_CAMERA_H
