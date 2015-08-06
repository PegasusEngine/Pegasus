/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Plane.cpp
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Plane type and operations

#include "Pegasus/Math/Plane.h"

namespace Pegasus {
namespace Math {


Plane::Plane()
:    mNormal(Vec3(0.0f, 1.0f, 0.0f)), mD(0.0f)
{
    // The constructor of mNormal has to be used,
    // otherwise its default constructor is called
}

//----------------------------------------------------------------------------------------

Plane::Plane(Vec3In normal, PFloat32 d)
:   mNormal(normal), mD(d)
{
    // The constructor of mNormal has to be used,
    // otherwise its default constructor is called
    PG_ASSERT(IsNormalized(mNormal, PEG_PLANE_NORMAL_EPSILON));
}

//----------------------------------------------------------------------------------------

Plane::Plane(Vec3In normal, Point3In p)
:   mNormal(normal)
{
    // The constructor of mNormal has to be used,
    // otherwise its default constructor is called
    PG_ASSERT(IsNormalized(mNormal, PEG_PLANE_NORMAL_EPSILON));
    SetOriginDistanceWithPoint(p);
}

//----------------------------------------------------------------------------------------

Plane::Plane(PFloat32 a, PFloat32 b, PFloat32 c, PFloat32 d)
:   mNormal(a, b, c), mD(d)
{
    // The constructor of mNormal has to be used,
    // otherwise its default constructor is called
    PG_ASSERT(IsNormalized(mNormal, PEG_PLANE_NORMAL_EPSILON));
}

//----------------------------------------------------------------------------------------

Plane::Plane(const PFloat32 coeffs[4])
:    mNormal(Vec3((const PFloat32 *)coeffs)), mD(coeffs[3])
{
    // The constructor of normal has to be used,
    // otherwise its default constructor is called
    PG_ASSERT(IsNormalized(mNormal, PEG_PLANE_NORMAL_EPSILON));
}

//----------------------------------------------------------------------------------------

Plane::Plane(const Plane & plane)
:    mNormal(plane.mNormal), mD(plane.mD)
{
}

//----------------------------------------------------------------------------------------

Plane & Plane::operator = (const Plane & plane)
{
    mNormal = plane.mNormal;
    mD = plane.mD;
    return *this;
}

//----------------------------------------------------------------------------------------

void Plane::SetCoeffs(const PFloat32 coeffs[4])
{
    mNormal.x = coeffs[0];
    mNormal.y = coeffs[1];
    mNormal.z = coeffs[2];
    mD = coeffs[3];
    PG_ASSERT(Length(mNormal) > PEG_PLANE_EPSILON);
    Normalize(mNormal);
}


}   // namespace Math
}   // namespace Pegasus
