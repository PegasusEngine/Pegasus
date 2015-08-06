/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AxisAlignedBoundingBox.h
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Axis-aligned bounding box type and operations

#ifndef PEGASUS_MATH_AXISALIGNEDBOUNDINGBOX_H
#define PEGASUS_MATH_AXISALIGNEDBOUNDINGBOX_H

#include "Pegasus/Math/Plane.h"
//#include "SceneGraph/Actor/Camera.h"

namespace Pegasus {
namespace Math {


//! Class defining an axis-aligned bounding box
class AxisAlignedBoundingBox
{
public:

    //! Default constructor (box of size 0 and center in (0, 0, 0))
    AxisAlignedBoundingBox();

    //! Constructor
    //! \param min Minimum coordinates of the bounding box
    //! \param max Maximum coordinates of the bounding box
    AxisAlignedBoundingBox(Point3In min, Point3In max);

    //! Copy constructor
    //! \param box Axis-aligned box to copy
    AxisAlignedBoundingBox(const AxisAlignedBoundingBox & box);

    //------------------------------------------------------------------------------------

public:

    //! Set the bounding box to a zero size (and centered in (0, 0, 0)
    void SetZero();

    //! Test if the bounding box has a zero size
    //! \return true if the bounding box has a zero size
    bool HasZeroSize() const;

    //! Set the bounds of the bounding box
    //! \param min Minimum coordinates of the bounding box
    //! \param max Maximum coordinates of the bounding box
    void SetMinMax(Point3In min, Point3In max);

    //! Set the center point and the size of the bounding box
    //! \param center Center point of the bounding box
    //! \param size Size of the box for the three axes (> 0.0f)
    void SetCenterSize(Point3In center, Vec3In size);

    //! Expand the bounding box in all directions
    //! \param additionalSpace Size of the space to add to all directions (> 0.0f)
    void Expand(PFloat32 additionalSpace);

    //------------------------------------------------------------------------------------

public:

    //! Get the minimum coordinates of the bounding box
    //! \return Minimum coordinates of the bounding box
    const Point3 & GetMin() const;

    //! Get the maximum coordinates of the bounding box
    //! \return Maximum coordinates of the bounding box
    const Point3 & GetMax() const;

    //! Get the center of the bounding box
    //! \return Center point of the bounding box
    const Point3 & GetCenter() const;

    //! Get the size of the bounding box
    //! \return Size of the box for the three axes
    const Vec3 & GetSize() const;

    //! Compute the coordinates of the bounding box corners
    //! \param corners Resulting coordinates of the bounding box corners
    void ComputeCornerCoordinates(Point3 corners[8]) const;

    //------------------------------------------------------------------------------------

public:

    //! Union with another axis-aligned bounding box
    //! \param box The other axis-aligned bounding box
    void Union(const AxisAlignedBoundingBox & box);

    //------------------------------------------------------------------------------------

public:

    //! Is the given point inside the bounding box
    //! \param point Point to test
    //! \return true if the point is inside the bounding box
    bool IsInside(Point3In point) const;

    //! Is the given point outside the bounding box
    //! \param point Point to test
    //! \return true if the point is outside the bounding box
    bool IsOutside(Point3In point) const;

    //! Is the current bounding box behind the given plane (with no contact)
    //! \param plane The plane to test
    //! \return true if the current box is strictly behind the plane
    bool IsBehind(const Plane & plane) const;

    //! Is the current bounding box inside the given camera view region
    //! \param camera Camera that has to be used for testing
    //! \return true if current bounding box is partially of fully inside the
    //!         view region of the camera
    //! \todo Implement camera support
    //bool IsInsideViewRegion(const Camera & camera) const;

    //! Is the transformed bounding box inside the given camera view region
    //! \param camera Camera that has to be used for testing
    //! \param mat Transformation matrix to apply to the bounding box before
    //!            testing if it is inside the camera view region
    //! \return true if current bounding box is partially of fully inside the
    //!         view region of the camera
    //! \todo Implement camera support
    //bool IsInsideViewRegionAfterTransform(const Camera & camera, Mat44In mat) const;

    //------------------------------------------------------------------------------------

public:

    //! Converts the parameters of the bounding box into a string for debugging
    //! \return String in the "(minX, minY, minZ) -> (maxX, maxY, maxZ)" format
    //! \todo Implement conversion to string
    //String ToString() const;

    //------------------------------------------------------------------------------------

private:

    //! Minimum coordinates of the bounding box
    mutable Point3 mMin;

    //! Maximum coordinates of the bounding box
    mutable Point3 mMax;

    //! Center point of the bounding box
    mutable Point3 mCenter;

    //! Size of the bounding box for the three axes
    mutable Vec3 mSize;

    //! true if the bounds (min, max) are defined
    mutable bool areMinMaxDefined;

    //! true if the center point and the size are defined
    mutable bool areCenterSizeDefined;
};

//----------------------------------------------------------------------------------------

//! Empty bounding box constant
extern const AxisAlignedBoundingBox N3DAABB_EMPTY;


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_AXISALIGNEDBOUNDINGBOX_H
