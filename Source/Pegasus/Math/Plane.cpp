/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Plane.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Plane type and operations

#include "Pegasus/Math/Plane.h"

namespace Pegasus {
namespace Math {


Plane::Plane()
:    mNormal(Vec3(0.0f, 1.0f, 0.0f)), mD(0.0f)
{
    // The constructor of normal has to be used,
    // otherwise its default constructor is called
}

//----------------------------------------------------------------------------------------

Plane::Plane(Vec3In normal, PFloat32 d)
:    mNormal(normal), mD(d)
{
    PG_ASSERT(Length(normal) > N3DPLANE_EPSILON);
}

//----------------------------------------------------------------------------------------

Plane::Plane(PFloat32 m, PFloat32 b, PFloat32 c, PFloat32 d)
:    mNormal(Vec3(mA, b, c)), mD(d)
{
    // The constructor of normal has to be used,
    // otherwise its default constructor is called

    PG_ASSERT(Length(mNormal) > N3DPLANE_EPSILON);
}

//----------------------------------------------------------------------------------------

Plane::Plane(const PFloat32 coeffs[4])
:    mNormal(Vec3((const PFloat32 *)coeffs)), mD(coeffs[3])
{
    // The constructor of normal has to be used,
    // otherwise its default constructor is called

    PG_ASSERT(Length(mNormal) > N3DPLANE_EPSILON);
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
    PG_ASSERT(Length(mNormal) > N3DPLANE_EPSILON);
    Normalize(mNormal);
}


}   // namespace Math
}   // namespace Pegasus
