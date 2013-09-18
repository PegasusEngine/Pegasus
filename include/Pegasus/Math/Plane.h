/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Plane.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Plane type and operations

#ifndef PEGASUS_MATH_PLANE_H
#define PEGASUS_MATH_PLANE_H

#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Math {


//! Value considered as zero for plane processing
#define N3DPLANE_EPSILON            1.0e-6f

//----------------------------------------------------------------------------------------

//! Class defining a 3D plane
//! A plane can be represented two ways : a.x + b.y + c.z + d = 0, or normal.P + d = 0,
//! P = (x, y, z) a point on the plane
class Plane
{
public:

    //! Default constructor (default plane: mA=0, mB=1, mC=0, mD=1)
    Plane();

    //! Constructor
    //! \warning The length has not to be null
    //! \param normal Normal to the plane (renormalized)
    //! \param d Distance from the origin
    Plane(Vec3In normal, PFloat32 d);

    //! Constructor
    //! \warning a, b, c cannot be zero at the same time
    //! \param a First plane coefficient
    //! \param b Second plane coefficient
    //! \param c Third plane coefficient
    //! \param d Fourth plane coefficient
    Plane(PFloat32 a, PFloat32 b, PFloat32 c, PFloat32 d);

    //! Constructor
    //! \warning The first three coefficients cannot be zero at the same time
    //! \param coeffs Four coefficients of the plane (as a, b, c and d)
    Plane(const PFloat32 coeffs[4]);

    //! Copy constructor
    //! \param plane Plane to copy
    Plane(const Plane & plane);

    //! Assignment operator
    //! \param plane Plane to copy
    //! \return Reference to the new plane
    Plane & operator = (const Plane & plane);

    //------------------------------------------------------------------------------------

    //! Set the normal of the plane
    //! \param normal Normal of the plane (internally normalized),
    //!               its length has not to be zero
    inline void SetNormal(Vec3In normal)
        { //PG_ASSERT(Length(normal) > N3DPLANE_EPSILON);
          PG_ASSERT(Length(normal) > 0.0f);
          mNormal = Normalize(normal);
        }

    //! Set the distance of the plane from the origin
    //! \param distance Distance of the plane from the origin
    inline void SetOriginDistance(PFloat32 distance)
        { mD = distance; }

    //! Set the distance of the plane from the origin
    //! by giving a 3D point that intersects the plane
    //! \warning The normal has to be valid since is used to get the origin distance
    //! \param point Point that intersects the plane
    inline void SetOriginDistanceWithPoint(Point3In point)
        { mD = -Dot(mNormal, point); }

    //! Set the four coefficients of the plane
    //! \param coeffs Four coefficients of the plane
    //!               (the three first components are normalized)
    void SetCoeffs(const PFloat32 coeffs[4]);

    //------------------------------------------------------------------------------------

    //! Get the normal of the plane
    //! \return Normalized normal of the plane
    inline const Vec3 & GetNormal() const
        { PG_ASSERT(Length(mNormal) > N3DPLANE_EPSILON);
          return mNormal;
        }

    //! Get the distance of the plane from the origin
    //! \return Distance of the plane from the origin
    inline PFloat32 GetOriginDistance() const
        { return mD; }

    //! Get the four coefficients of the plane
    //! \return Four coefficients of the plane
    inline const PFloat32 * const GetCoeffs() const
        { return mCoeffs; }

    //------------------------------------------------------------------------------------

    //! Get the distance from a point to the plane
    //! \param point Point to test
    //! \return Distance of the point from the plane, positive if in front
    inline PFloat32 DistanceOfPoint(Point3In point) const
        { return mNormal.x * point.x + mNormal.y * point.y + mNormal.z * point.z + mD; }

    //! Get the distance from a point to the plane
    //! \param x X component of the point to test
    //! \param y Y component of the point to test
    //! \param z Z component of the point to test
    //! \return Distance of the point from the plane, positive if in front
    inline PFloat32 DistanceOfPoint(PFloat32 x, PFloat32 y, PFloat32 z) const
        { return mNormal.x * x + mNormal.y * y + mNormal.z * z + mD; }

    //! Is the given point in front of the plane
    //! \param point Point to test
    //! \return true if the point is in front of the plane
    inline bool IsInFront(Point3In point) const
        { return (mNormal.x * point.x + mNormal.y * point.y + mNormal.z * point.z > -mD); }

    //! Is the given point in front of the plane
    //! \param x X component of the point to test
    //! \param y Y component of the point to test
    //! \param z Z component of the point to test
    //! \return true if the point is in front of the plane
    inline bool IsInFront(PFloat32 x, PFloat32 y, PFloat32 z) const
        { return (mNormal.x * x + mNormal.y * y + mNormal.z * z > -mD); }

    //! Is the given point in the plane (in fact at a minimal distance from it)
    //! \param point Point to test
    //! \param eps Distance of the point from the plane to consider it as inside
    //! \return true if the point is in the plane
    inline bool IsInPlane(Point3In point, PFloat32 eps = N3DPLANE_EPSILON) const
        { return (Abs(mNormal.x * point.x + mNormal.y * point.y + mNormal.z * point.z + mD) <= eps); }

    //! Is the given point in the plane (in fact at a minimal distance from it)
    //! \param x X component of the point to test
    //! \param y Y component of the point to test
    //! \param z Z component of the point to test
    //! \param eps Distance of the point from the plane to consider it as inside
    //! \return true if the point is in the plane
    inline bool IsInPlane(PFloat32 x, PFloat32 y, PFloat32 z, PFloat32 eps = N3DPLANE_EPSILON) const
        { return (Abs(mNormal.x * x + mNormal.y * y + mNormal.z * z + mD) <= eps); }

    //! Is the given point in the back of the plane
    //! \param point Point to test
    //! \return true if the point is in the back of the plane
    inline bool IsInBack(Point3In point) const
        { return (mNormal.x * point.x + mNormal.y * point.y + mNormal.z * point.z < -mD); }

    //! Is the given point in the back of the plane
    //! \param x X component of the point to test
    //! \param y Y component of the point to test
    //! \param z Z component of the point to test
    //! \return true if the point is in the back of the plane
    inline bool IsInBack(PFloat32 x, PFloat32 y, PFloat32 z) const
        { return (mNormal.x * x + mNormal.y * y + mNormal.z * z < -mD);  }

    //------------------------------------------------------------------------------------

private:

    union
    {
        struct
        {
            union
            {
                struct { Vec3 mNormal; };           //!< Vector representation of the normal
                struct { PFloat32 mA, mB, mC; };    //!< Coefficients of the normal
            };
            //! \todo Repeat mD as done for Vec4
            PFloat32 mD;                            //!< Distance from the origin
        };

        PFloat32 mCoeffs[4];                        //!< Vector representation of all coefficients
    };
};

//----------------------------------------------------------------------------------------

//! Function input type for a plane
typedef const Plane & PlaneIn;

//! Function input/output type for a plane
typedef       Plane & PlaneInOut;

//! Function return type for a plane
typedef       Plane   PlaneReturn;


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_PLANE_H
