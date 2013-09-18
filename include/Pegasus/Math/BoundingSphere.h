/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BoundingSphere.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Bounding sphere type and operations

#ifndef PEGASUS_MATH_BOUNDINGSPHERE_H
#define PEGASUS_MATH_BOUNDINGSPHERE_H

#include "Pegasus/Math/Plane.h"
//#include "SceneGraph/Actor/Camera.h"

namespace Pegasus {
namespace Math {


//! Class defining a bounding sphere
class BoundingSphere
{
public:

    //! Default constructor (sphere centered in (0,0,0) and of radius 1)
    BoundingSphere();

    //! Constructor
    //! \param center Center point of the sphere
    //! \param radius Radius of the sphere (> 0.0f)
    BoundingSphere(Point3In center, PFloat32 radius);

    //! Copy constructor
    //! \param sphere Bounding sphere to copy
    BoundingSphere(const BoundingSphere & sphere);

    //------------------------------------------------------------------------------------

    //! Set the center of the bounding sphere
    //! \param center Center point of the bounding sphere
    inline void SetCenter(Point3In center) { mCenter = center; }

    //! Set the radius of the bounding sphere
    //! \param radius Radius of the bounding sphere (> 0.0f)
    inline void SetRadius(PFloat32 radius)
        { PG_ASSERT(radius > 0.0f); mRadius = radius; }

    //------------------------------------------------------------------------------------

    //! Get the center of the bounding sphere
    //! \return Center point of the bounding sphere
    inline const Point3 & GetCenter() const { return mCenter; }

    //! Get the radius of the bounding sphere
    //! \return Radius of the bounding sphere (> 0.0f)
    inline PFloat32 GetRadius() const { return mRadius; }

    //------------------------------------------------------------------------------------

    //! Is the given point inside the bounding sphere
    //! \param point Point to test
    //! \return true if the point is inside the bounding sphere
    inline bool IsInside(Point3In point) const
        { return Length(point - mCenter) <= mRadius; }

    //! Is the given point on the sphere (in fact at a minimal distance from the surface)
    //! \param point Point to test
    //! \param eps Distance of the point from the sphere surface to consider it on it
    //! \return true if the point is on the sphere
    inline bool IsOnSphere(Point3In point, PFloat32 eps = N3DPLANE_EPSILON) const
        { return Abs(Length(point - mCenter) - mRadius) <= eps; }

    //! Is the given point outside the bounding sphere
    //! \param point Point to test
    //! \return true if the point is outside the bounding sphere
    inline bool IsOutside(Point3In point) const
        { return Length(point - mCenter) > mRadius; }

    //! Is the current bounding sphere in front of the given plane (with no intersection)
    //! \param plane The plane to test
    //! \return true if the current sphere is strictly in front of the plane
    inline bool IsInFront(const Plane & plane) const
        { return plane.DistanceOfPoint(mCenter) > mRadius; }

    //! Is the current bounding sphere behind the given plane (with no intersection)
    //! \param plane The plane to test
    //! \return true if the current sphere is strictly behind the plane
    inline bool IsBehind(const Plane & plane) const
        { return plane.DistanceOfPoint(mCenter) < -mRadius; }

    //! Is the current bounding sphere strictly inside the given camera view region
    //! \param camera Camera that has to be used for testing
    //! \return true if current bounding sphere is fully inside the view region
    //!         of the camera
    //! \todo Implement camera support
    //bool IsFullyInsideViewRegion(const Camera & camera) const;

    //! Is the current bounding sphere inside the given camera view region
    //! \param camera Camera that has to be used for testing
    //! \return true if current bounding sphere is partially of fully inside the
    //!         view region of the camera
    //! \todo Implement camera support
    //bool IsInsideViewRegion(const Camera & camera) const;

    //------------------------------------------------------------------------------------

private:

    //! Center point of the bounding sphere
    Point3 mCenter;

    //! Radius of the bounding sphere (> 0.0f)
    PFloat32 mRadius;
};


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_BOUNDINGSPHERE_H
