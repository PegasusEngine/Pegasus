/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Vector.cpp
//! \author Kevin Boulanger
//! \date   17th September 2013
//! \brief  Vector types and operations

#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Math {


Vec2Return Min(const Vec2 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec2 min(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        min = Min(min, vec[j]);
    }

    return min;
}

//----------------------------------------------------------------------------------------

Vec3Return Min(const Vec3 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec3 min(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        min = Min(min, vec[j]);
    }

    return min;
}

//----------------------------------------------------------------------------------------

Vec4Return Min(const Vec4 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec4 min(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        min = Min(min, vec[j]);
    }

    return min;
}

//----------------------------------------------------------------------------------------

Vec2Return Max(const Vec2 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec2 max(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        max = Max(max, vec[j]);
    }

    return max;
}

//----------------------------------------------------------------------------------------

Vec3Return Max(const Vec3 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec3 max(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        max = Max(max, vec[j]);
    }

    return max;
}

//----------------------------------------------------------------------------------------

Vec4Return Max(const Vec4 * vec, PUInt32 n)
{
    PG_ASSERT(vec);
    PG_ASSERT(n > 0);

    Vec4 max(vec[0]);
    for (PUInt32 j = 1; j < n; j++)
    {
        max = Max(max, vec[j]);
    }

    return max;
}

//----------------------------------------------------------------------------------------

Vec2Return PolarToCartesian(PFloat32 radius, PFloat32 rho)
{
    return Vec2(radius * Cos(rho), radius * Sin(rho));
}

//----------------------------------------------------------------------------------------

Vec2Return PolarToCartesian(PFloat32 rho)
{
    return Vec2(Cos(rho), Sin(rho));
}

//----------------------------------------------------------------------------------------

void CartesianToPolar(PFloat32 & radius, PFloat32 & rho, Vec2In coords)
{
    radius = Length(coords);
    rho = Atan2(coords.z, coords.x);
    if (rho < 0.0f)
    {
        rho += P_2_PI;
    }
}

//----------------------------------------------------------------------------------------

void CartesianToPolar(PFloat32 & radius, PFloat32 & rho, Vec3In coords)
{
    radius = Sqrt(coords.x * coords.x + coords.z * coords.z);
    rho = Atan2(coords.z, coords.x);
    if (rho < 0.0f)
    {
        rho += P_2_PI;
    }
}

//----------------------------------------------------------------------------------------

Vec3Return CylindricalToCartesian(PFloat32 radius, PFloat32 rho, PFloat32 height)
{
    return Vec3(radius * Cos(rho), height, radius * Sin(rho));
}

//----------------------------------------------------------------------------------------

Vec3Return CylindricalToCartesian(PFloat32 rho, PFloat32 height)
{
    return Vec3(Cos(rho), height, Sin(rho));
}

//----------------------------------------------------------------------------------------

Vec3Return CylindricalToCartesian(PFloat32 rho)
{
    return Vec3(Cos(rho), 0.0f, Sin(rho));
}

//----------------------------------------------------------------------------------------

void CartesianToCylindrical(PFloat32 & radius, PFloat32 & rho, PFloat32 & height, Vec3In coords)
{
    radius = Sqrt(coords.x * coords.x + coords.z * coords.z);
    rho = Atan2(coords.z, coords.x);
    if (rho < 0.0f)
    {
        rho += P_2_PI;
    }
    height = coords.y;
}

//----------------------------------------------------------------------------------------

Vec3Return SphericalToCartesian(PFloat32 radius, PFloat32 phi, PFloat32 theta)
{
    const PFloat32 stheta = Sin(theta);
    return Vec3(radius * stheta * Cos(phi),
                radius * Cos(theta),
                radius * stheta * Sin(phi));
}

//----------------------------------------------------------------------------------------

Vec3Return SphericalToCartesian(PFloat32 phi, PFloat32 theta)
{
    const PFloat32 stheta = Sin(theta);
    return Vec3(stheta * Cos(phi),
                Cos(theta),
                stheta * Sin(phi));
}

//----------------------------------------------------------------------------------------

void CartesianToSpherical(PFloat32 & radius, PFloat32 & phi, PFloat32 & theta, Vec3In coords)
{
    radius = Length(coords);

    phi = Atan2(coords.z, coords.x);
    if (phi < 0.0f)
    {
        phi += P_2_PI;
    }

    if (radius == 0.0f)
    {
        theta = 0.0f;
    }
    else
    {
        theta = Acos(coords.y / radius);
    }
}

//----------------------------------------------------------------------------------------

void CartesianToSphericalPhi(PFloat32 & phi, Vec3In coords)
{
    phi = Atan2(coords.z, coords.x);
    if (phi < 0.0f)
    {
        phi += P_2_PI;
    }
}

//----------------------------------------------------------------------------------------

void CartesianToSphericalTheta(PFloat32 & theta, Vec3In coords)
{
    const float radius = Length(coords);
    if (radius == 0.0f)
    {
        theta = 0.0f;
    }
    else
    {
        theta = Acos(coords.y / radius);
    }
}

//----------------------------------------------------------------------------------------

}   // namespace Math
}   // namespace Pegasus
