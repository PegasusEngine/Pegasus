/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Intersections.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Functions to test intersections between geometric primitives

#ifndef PEGASUS_MATH_INTERSECTIONS_H
#define PEGASUS_MATH_INTERSECTIONS_H

#include "Pegasus/Math/Ray.h"
#include "Pegasus/Math/Plane.h"

namespace Pegasus {
namespace Math {


// Functions that test the intersection between geometric primitives without giving
// the exact coordinates (faster)

//! Test the intersection between a ray and a plane.
//! true most of the time, except when the ray direction is parallel to the plane
//! and the origin point not in the plane
//! \param ray Input ray
//! \param plane Input plane
//! \return true if the ray and the plane intersect
bool AreIntersecting(RayIn ray, PlaneIn plane);

//----------------------------------------------------------------------------------------

// Functions that give the intersection point between geometric primitives

//! Get the intersection point between a ray and a plane
//! \warning The ray direction has to be non-parallel to the plane
//! \param ray Input ray
//! \param plane Input plane
//! \return Resulting intersection point
Point3Return GetIntersection(RayIn ray, PlaneIn plane);

//! Get the intersection point between a ray and a plane
//! \warning The ray direction has to be non-parallel to the plane
//! \param result Resulting intersection point
//! \param ray Input ray
//! \param plane Input plane
inline void GetIntersection(Point3InOut result, RayIn ray, PlaneIn plane)
    {   result = GetIntersection(ray, plane);   }

//----------------------------------------------------------------------------------------

//! Value considered as zero for intersection processing
#define N3DINTERSECTIONS_EPSILON                1.0e-6f


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_INTERSECTIONS_H
