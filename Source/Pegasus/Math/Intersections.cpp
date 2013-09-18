/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Intersections.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Functions to test intersections between geometric primitives

#include "Pegasus/Math/Intersections.h"

namespace Pegasus {
namespace Math {


bool AreIntersecting(RayIn ray, PlaneIn plane)
{
    return    plane.IsInPlane(ray.GetOrigin())
           || (Abs(Dot(plane.GetNormal(), ray.GetDirection())) > N3DINTERSECTIONS_EPSILON);
}

//----------------------------------------------------------------------------------------

Point3Return GetIntersection(RayIn ray, PlaneIn plane)
{
    PG_ASSERT(AreIntersecting(ray, plane));

    const float t = ( -plane.GetOriginDistance() - Dot(plane.GetNormal(), ray.GetOrigin()) )
                        / Dot(plane.GetNormal(), ray.GetDirection());
    return ray.GetPointFromDistance(t);
}


}   // namespace Math
}   // namespace Pegasus
