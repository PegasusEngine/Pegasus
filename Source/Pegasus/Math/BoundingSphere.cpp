/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BoundingSphere.cpp
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Bounding sphere type and operations

#include "Pegasus/Math/BoundingSphere.h"

namespace Pegasus {
namespace Math {

    
BoundingSphere::BoundingSphere()
:   mRadius(1.0f)
{
    SetZero(mCenter);
}

//----------------------------------------------------------------------------------------

BoundingSphere::BoundingSphere(Point3In center, PFloat32 radius)
:   mCenter(center),
    mRadius(radius > 0.0f ? radius : 1.0f)
{
    PG_ASSERT(radius > 0.0f);
}

//----------------------------------------------------------------------------------------

BoundingSphere::BoundingSphere(const BoundingSphere & sphere)
:   mCenter(sphere.mCenter),
    mRadius(sphere.mRadius)
{
}

//----------------------------------------------------------------------------------------

//! \todo Implement camera support
//bool BoundingSphere::IsFullyInsideViewRegion(const Camera & camera) const
//{
//    return    IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_NEAR))
//           && IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_FAR))
//           && IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_LEFT))
//           && IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_RIGHT))
//           && IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_BOTTOM))
//           && IsInFront(camera.GetPlane(Camera::N3DCAMERAPLANE_TOP));
//}

//----------------------------------------------------------------------------------------

//! \todo Implement camera support
//bool BoundingSphere::IsInsideViewRegion(const Camera & camera) const
//{
    //return !(   IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_NEAR  ))
    //         || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_FAR   ))
    //         || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_LEFT  ))
    //         || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_RIGHT ))
    //         || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_BOTTOM))
    //         || IsBehind(camera.GetPlane(Camera::N3DCAMERAPLANE_TOP   )) );
//}


}   // namespace Math
}   // namespace Pegasus
