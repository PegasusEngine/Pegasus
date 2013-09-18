/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Ray.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Ray type and operations

#ifndef PEGASUS_MATH_RAY_H
#define PEGASUS_MATH_RAY_H

#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Math {


//! Value considered as zero for ray processing
#define RAY_EPSILON             1.0e-6f

//----------------------------------------------------------------------------------------

//! Class defining a ray (origin point P and direction D)
class Ray
{
public:

    //! Default constructor (default ray: P=(0,0,0), D(0,0,-1))
    Ray();

    //! Constructor
    //! \warning The length of the direction vector has not to be null
    //! \param origin Origin point of the ray
    //! \param direction Direction of the ray (renormalized)
    Ray(Point3In origin, Vec3In direction);

    //! Constructor
    //! \warning dx, dy, dz cannot be zero at the same time
    //! \param x Coordinate of the origin point (X component)
    //! \param y Coordinate of the origin point (Y component)
    //! \param z Coordinate of the origin point (Z component)
    //! \param dx Coordinate of the direction vector (X component)
    //! \param dy Coordinate of the direction vector (Y component)
    //! \param dz Coordinate of the direction vector (Z component)
    Ray(PFloat32 x, PFloat32 y, PFloat32 z, PFloat32 dx, PFloat32 dy, PFloat32 dz);

    //! Copy constructor
    //! \param ray Ray to copy
    Ray(const Ray & ray);

    //! Assignment operator
    //! \param ray Ray to copy
    //! \return Reference to the new ray
    Ray & operator = (const Ray & ray);

    //------------------------------------------------------------------------------------

    //! Set the origin point of the ray
    //! \param origin Origin point of the ray
    inline void SetOrigin(Point3In origin) { mOrigin = origin; }

    //! Set the direction vector of the ray
    //! \param direction Direction vector of the ray (internally normalized),
    //!                  its length has not to be zero
    inline void SetDirection(Vec3In direction)
        {
            PG_ASSERT(Length(direction) > RAY_EPSILON);
            mDirection = direction;
        }

    //! Set the origin point and direction of the ray from a pair of points
    //! \param origin Origin point of the ray
    //! \param point Point on the ray (different from the origin point)
    inline void SetFromPoints(Point3In origin, Point3In point)
        {
            PG_ASSERT(Length(point - origin) > RAY_EPSILON);
            mOrigin = origin;
            mDirection = point;
            mDirection -= mOrigin;
            Normalize(mDirection);
        }

    //------------------------------------------------------------------------------------

    //! Get the origin point of the ray
    //! \return Origin point of the ray
    inline const Point3Return & GetOrigin() const { return mOrigin; }

    //! Get the direction of the ray
    //! \return Normalized direction of the ray
    inline const Vec3Return & GetDirection() const { return mDirection; }

    //------------------------------------------------------------------------------------

    //! Get a point on the ray based on its distance from the origin
    //! \param distance Distance from the origin point of the ray (can be negative)
    //! \return Resulting point on the ray
    inline Point3Return GetPointFromDistance(PFloat32 distance) const
        { return mOrigin + distance * mDirection; }

    //! Get the distance from the origin of the orthogonal projection
    //! of a point onto the the ray
    //! \param point Point to test
    //! \return Distance from the origin of the orthogonal projection of point
    inline float DistanceOfPointProjectionFromOrigin(Point3In point) const
        { return Dot(point - mOrigin, mDirection); }

    //! Get the orthogonal projection of a point onto the the ray
    //! \param point Point to test
    //! \return Resulting orthogonally projected point
    inline Point3Return Project(Point3In point) const
        { return mOrigin + DistanceOfPointProjectionFromOrigin(point) * mDirection; }

    //------------------------------------------------------------------------------------

private:

    //! Origin point
    Point3 mOrigin;

    //! Normalized direction
    Vec3 mDirection;
};

//----------------------------------------------------------------------------------------

//! Function input type for a ray
typedef const Ray & RayIn;

//! Function input/output type for a ray
typedef       Ray & RayInOut;

//! Function return type for a ray
typedef       Ray   RayReturn;


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_RAY_H
