/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Ray.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Ray type and operations

#include "Pegasus/Math/Ray.h"

namespace Pegasus {
namespace Math {


Ray::Ray()
:   mOrigin(0.0f),
    mDirection(0.0f, 0.0f, 1.0f)
{
}

//----------------------------------------------------------------------------------------

Ray::Ray(Point3In origin, Vec3In direction)
:   mOrigin(origin),
    mDirection(direction)
{
    PG_ASSERT(Length(direction) > RAY_EPSILON);
}

//----------------------------------------------------------------------------------------

Ray::Ray(PFloat32 x, PFloat32 y, PFloat32 z, PFloat32 dx, PFloat32 dy, PFloat32 dz)
:   mOrigin(x, y, z),
    mDirection(dx, dy, dz)
{
    PG_ASSERT(Length(mDirection) > RAY_EPSILON);
}

//----------------------------------------------------------------------------------------

Ray::Ray(const Ray & ray)
:   mOrigin(ray.mOrigin),
    mDirection(ray.mDirection)
{
}

//----------------------------------------------------------------------------------------

Ray & Ray::operator = (const Ray & ray)
{
    mOrigin = ray.mOrigin;
    mDirection = ray.mDirection;
    return *this;
}


}   // namespace Math
}   // namespace Pegasus
