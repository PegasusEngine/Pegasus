/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Quaternion.cpp
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Quaternion types and operations

#include "Pegasus/Math/Quaternion.h"
#include "Pegasus/Math/Constants.h"

namespace Pegasus {
namespace Math {


Quaternion::Quaternion(Vec3In axis, PFloat32 angle)
{
    AxisAngleToQuaternion(*this, axis, angle);
}

//----------------------------------------------------------------------------------------

Quaternion::Quaternion(Mat33In mat)
{
    Mat33ToQuaternion(*this, mat);
}

//----------------------------------------------------------------------------------------

Quaternion::Quaternion(Mat44In mat)
{
    Mat44ToQuaternion(*this, mat);
}

//----------------------------------------------------------------------------------------

void AxisAngleToQuaternion(QuaternionInOut quat, Vec3In axis, PFloat32 angle)
{
    if (   IsZero(axis)
        || ((angle < PFLOAT_EPSILON) && (angle > -PFLOAT_EPSILON)) )
    {
        SetDefaultQuaternion(quat);
        return;
    }

    const Vec3 nAxis = Normalize(axis);
    angle *= 0.5f;

    quat.w = Cos(angle);
    const PFloat32 sinAngle = Sin(angle);
    quat.x = sinAngle * nAxis.x;
    quat.y = sinAngle * nAxis.y;
    quat.z = sinAngle * nAxis.z;
}

//----------------------------------------------------------------------------------------

Quaternion AxisAngleToQuaternion(Vec3In axis, PFloat32 angle)
{
    if (   IsZero(axis)
        || ((angle < PFLOAT_EPSILON) && (angle > -PFLOAT_EPSILON)) )
     {
         return Quaternion();
    }

    const Vec3 nAxis = Normalize(axis);
    angle *= 0.5f;

    const PFloat32 sinAngle = Sin(angle);
    return Quaternion(Cos(angle),
                      sinAngle * nAxis.x,
                      sinAngle * nAxis.y,
                      sinAngle * nAxis.z);
}

//----------------------------------------------------------------------------------------

void QuaternionToAxisAngle(Vec3InOut axis, PFloat32 & angle, QuaternionIn quat)
{
    // Angle computation
    angle = 2.0f * Acos(Clamp(quat.w, -1.0f, 1.0f));

    // Axis computation
    axis.x = quat.x;
    axis.y = quat.y;
    axis.z = quat.z;
    if ((axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f))
    {
        Normalize(axis);
    }
}

//----------------------------------------------------------------------------------------

void QuaternionToMat33(Mat33InOut mat, QuaternionIn quat)
{
    const PFloat32 qx2  = quat.x * quat.x;
    const PFloat32 qy2  = quat.y * quat.y;
    const PFloat32 qz2  = quat.z * quat.z;
    const PFloat32 qwqx = quat.w * quat.x;
    const PFloat32 qwqy = quat.w * quat.y;
    const PFloat32 qwqz = quat.w * quat.z;
    const PFloat32 qxqy = quat.x * quat.y;
    const PFloat32 qxqz = quat.x * quat.z;
    const PFloat32 qyqz = quat.y * quat.z;
    mat.m11 = 1.0f - 2.0f * (qy2 + qz2);
    mat.m12 = 2.0f * (qxqy - qwqz);
    mat.m13 = 2.0f * (qxqz + qwqy);
    mat.m21 = 2.0f * (qxqy + qwqz);
    mat.m22 = 1.0f - 2.0f * (qx2 + qz2);
    mat.m23 = 2.0f * (qyqz - qwqx);
    mat.m31 = 2.0f * (qxqz - qwqy);
    mat.m32 = 2.0f * (qyqz + qwqx);
    mat.m33 = 1.0f - 2.0f * (qx2 + qy2);
}

//----------------------------------------------------------------------------------------

void QuaternionToMat33T(Mat33InOut mat, QuaternionIn quat)
{
    const PFloat32 qx2  = quat.x * quat.x;
    const PFloat32 qy2  = quat.y * quat.y;
    const PFloat32 qz2  = quat.z * quat.z;
    const PFloat32 qwqx = quat.w * quat.x;
    const PFloat32 qwqy = quat.w * quat.y;
    const PFloat32 qwqz = quat.w * quat.z;
    const PFloat32 qxqy = quat.x * quat.y;
    const PFloat32 qxqz = quat.x * quat.z;
    const PFloat32 qyqz = quat.y * quat.z;
    mat.m11 = 1.0f - 2.0f * (qy2 + qz2);
    mat.m12 = 2.0f * (qxqy + qwqz);
    mat.m13 = 2.0f * (qxqz - qwqy);
    mat.m21 = 2.0f * (qxqy - qwqz);
    mat.m22 = 1.0f - 2.0f * (qx2 + qz2);
    mat.m23 = 2.0f * (qyqz + qwqx);
    mat.m31 = 2.0f * (qxqz + qwqy);
    mat.m32 = 2.0f * (qyqz - qwqx);
    mat.m33 = 1.0f - 2.0f * (qx2 + qy2);
}

//----------------------------------------------------------------------------------------

void QuaternionToMat44(Mat44InOut mat, QuaternionIn quat)
{
    const PFloat32 qx2  = quat.x * quat.x;
    const PFloat32 qy2  = quat.y * quat.y;
    const PFloat32 qz2  = quat.z * quat.z;
    const PFloat32 qwqx = quat.w * quat.x;
    const PFloat32 qwqy = quat.w * quat.y;
    const PFloat32 qwqz = quat.w * quat.z;
    const PFloat32 qxqy = quat.x * quat.y;
    const PFloat32 qxqz = quat.x * quat.z;
    const PFloat32 qyqz = quat.y * quat.z;
    mat.m11 = 1.0f - 2.0f * (qy2 + qz2);
    mat.m12 = 2.0f * (qxqy - qwqz);
    mat.m13 = 2.0f * (qxqz + qwqy);
    mat.m14 = 0.0f;
    mat.m21 = 2.0f * (qxqy + qwqz);
    mat.m22 = 1.0f - 2.0f * (qx2 + qz2);
    mat.m23 = 2.0f * (qyqz - qwqx);
    mat.m24 = 0.0f;
    mat.m31 = 2.0f * (qxqz - qwqy);
    mat.m32 = 2.0f * (qyqz + qwqx);
    mat.m33 = 1.0f - 2.0f * (qx2 + qy2);
    mat.m34 = 0.0f;
    mat.m41 = 0.0f;
    mat.m42 = 0.0f;
    mat.m43 = 0.0f;
    mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void QuaternionToMat44T(Mat44InOut mat, QuaternionIn quat)
{
    const PFloat32 qx2 = quat.x  * quat.x;
    const PFloat32 qy2 = quat.y  * quat.y;
    const PFloat32 qz2 = quat.z  * quat.z;
    const PFloat32 qwqx = quat.w * quat.x;
    const PFloat32 qwqy = quat.w * quat.y;
    const PFloat32 qwqz = quat.w * quat.z;
    const PFloat32 qxqy = quat.x * quat.y;
    const PFloat32 qxqz = quat.x * quat.z;
    const PFloat32 qyqz = quat.y * quat.z;
    mat.m11 = 1.0f - 2.0f * (qy2 + qz2);
    mat.m12 = 2.0f * (qxqy + qwqz);
    mat.m13 = 2.0f * (qxqz - qwqy);
    mat.m14 = 0.0f;
    mat.m21 = 2.0f * (qxqy - qwqz);
    mat.m22 = 1.0f - 2.0f * (qx2 + qz2);
    mat.m23 = 2.0f * (qyqz + qwqx);
    mat.m24 = 0.0f;
    mat.m31 = 2.0f * (qxqz + qwqy);
    mat.m32 = 2.0f * (qyqz - qwqx);
    mat.m33 = 1.0f - 2.0f * (qx2 + qy2);
    mat.m34 = 0.0f;
    mat.m41 = 0.0f;
    mat.m42 = 0.0f;
    mat.m43 = 0.0f;
    mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void Mat33ToQuaternion(QuaternionInOut quat, Mat33In mat)
{
    PFloat32 s;

    const PFloat32 t = 1.0f + mat.m11 + mat.m22 + mat.m33;
    if (t > 0.0f)
    {
        s = 0.5f * RcpSqrt(t);
        quat.w = 0.25f / s;
        quat.x = (mat.m32 - mat.m23) * s;
        quat.y = (mat.m13 - mat.m31) * s;
        quat.z = (mat.m21 - mat.m12) * s;
    }

    else if ((mat.m11 >= mat.m22) && (mat.m11 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f + mat.m11 - mat.m22 - mat.m33);
        quat.w = (mat.m32 + mat.m23) * s;
        quat.x = 0.5f * s;
        quat.y = (mat.m21 + mat.m12) * s;
        quat.z = (mat.m13 + mat.m31) * s;
    }

    else if ((mat.m22 >= mat.m11) && (mat.m22 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 + mat.m22 - mat.m33);
        quat.w = (mat.m13 + mat.m31) * s;
        quat.x = (mat.m21 + mat.m12) * s;
        quat.y = 0.5f * s;
        quat.z = (mat.m32 + mat.m23) * s;
    }

    else
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 - mat.m22 + mat.m33);
        quat.w = (mat.m21 + mat.m12) * s;
        quat.x = (mat.m13 + mat.m31) * s;
        quat.y = (mat.m32 + mat.m23) * s;
        quat.z = 0.5f * s;
    }

    Normalize(quat);
}

//----------------------------------------------------------------------------------------

void Mat33TToQuaternion(QuaternionInOut quat, Mat33In mat)
{
    PFloat32 s;

    const PFloat32 t = 1.0f + mat.m11 + mat.m22 + mat.m33;
    if (t > 0.0f)
    {
        s = 0.5f * RcpSqrt(t);
        quat.w = 0.25f / s;
        quat.x = (mat.m23 - mat.m32) * s;
        quat.y = (mat.m31 - mat.m13) * s;
        quat.z = (mat.m12 - mat.m21) * s;
    }

    else if ((mat.m11 >= mat.m22) && (mat.m11 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f + mat.m11 - mat.m22 - mat.m33);
        quat.w = (mat.m23 + mat.m32) * s;
        quat.x = 0.5f * s;
        quat.y = (mat.m12 + mat.m21) * s;
        quat.z = (mat.m31 + mat.m13) * s;
    }

    else if ((mat.m22 >= mat.m11) && (mat.m22 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 + mat.m22 - mat.m33);
        quat.w = (mat.m31 + mat.m13) * s;
        quat.x = (mat.m12 + mat.m21) * s;
        quat.y = 0.5f * s;
        quat.z = (mat.m23 + mat.m32) * s;
    }

    else
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 - mat.m22 + mat.m33);
        quat.w = (mat.m12 + mat.m21) * s;
        quat.x = (mat.m31 + mat.m13) * s;
        quat.y = (mat.m23 + mat.m32) * s;
        quat.z = 0.5f * s;
    }

    Normalize(quat);
}

//----------------------------------------------------------------------------------------

void Mat44ToQuaternion(QuaternionInOut quat, Mat44In mat)
{
    PFloat32 s;

    const PFloat32 t = 1.0f + mat.m11 + mat.m22 + mat.m33;
    if (t > 0.0f)
    {
        s = 0.5f * RcpSqrt(t);
        quat.w = 0.25f / s;
        quat.x = (mat.m32 - mat.m23) * s;
        quat.y = (mat.m13 - mat.m31) * s;
        quat.z = (mat.m21 - mat.m12) * s;
    }

    else if ((mat.m11 >= mat.m22) && (mat.m11 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f + mat.m11 - mat.m22 - mat.m33);
        quat.w = (mat.m32 + mat.m23) * s;
        quat.x = 0.5f * s;
        quat.y = (mat.m21 + mat.m12) * s;
        quat.z = (mat.m13 + mat.m31) * s;
    }

    else if ((mat.m22 >= mat.m11) && (mat.m22 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 + mat.m22 - mat.m33);
        quat.w = (mat.m13 + mat.m31) * s;
        quat.x = (mat.m21 + mat.m12) * s;
        quat.y = 0.5f * s;
        quat.z = (mat.m32 + mat.m23) * s;
    }

    else
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 - mat.m22 + mat.m33);
        quat.w = (mat.m21 + mat.m12) * s;
        quat.x = (mat.m13 + mat.m31) * s;
        quat.y = (mat.m32 + mat.m23) * s;
        quat.z = 0.5f * s;
    }

    Normalize(quat);
}

//----------------------------------------------------------------------------------------

void Mat44TToQuaternion(QuaternionInOut quat, Mat44In mat)
{
    PFloat32 s;

    const PFloat32 t = 1.0f + mat.m11 + mat.m22 + mat.m33;
    if (t > 0.0f)
    {
        s = 0.5f * RcpSqrt(t);
        quat.w = 0.25f / s;
        quat.x = (mat.m23 - mat.m32) * s;
        quat.y = (mat.m31 - mat.m13) * s;
        quat.z = (mat.m12 - mat.m21) * s;
    }

    else if ((mat.m11 >= mat.m22) && (mat.m11 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f + mat.m11 - mat.m22 - mat.m33);
        quat.w = (mat.m23 + mat.m32) * s;
        quat.x = 0.5f * s;
        quat.y = (mat.m12 + mat.m21) * s;
        quat.z = (mat.m31 + mat.m13) * s;
    }

    else if ((mat.m22 >= mat.m11) && (mat.m22 >= mat.m33))
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 + mat.m22 - mat.m33);
        quat.w = (mat.m31 + mat.m13) * s;
        quat.x = (mat.m12 + mat.m21) * s;
        quat.y = 0.5f * s;
        quat.z = (mat.m23 + mat.m32) * s;
    }

    else
    {
        s = 0.5f * RcpSqrt(1.0f - mat.m11 - mat.m22 + mat.m33);
        quat.w = (mat.m12 + mat.m21) * s;
        quat.x = (mat.m31 + mat.m13) * s;
        quat.y = (mat.m23 + mat.m32) * s;
        quat.z = 0.5f * s;
    }

    Normalize(quat);
}

//----------------------------------------------------------------------------------------

void LeftMult(QuaternionInOut quat1, QuaternionIn quat2)
{
    const PFloat32 w = quat2.w * quat1.w - quat2.x * quat1.x - quat2.y * quat1.y - quat2.z * quat1.z;
    const PFloat32 x = quat2.w * quat1.x + quat2.x * quat1.w + quat2.y * quat1.z - quat2.z * quat1.y;
    const PFloat32 y = quat2.w * quat1.y - quat2.x * quat1.z + quat2.y * quat1.w + quat2.z * quat1.x;
    quat1.z          = quat2.w * quat1.z + quat2.x * quat1.y - quat2.y * quat1.x + quat2.z * quat1.w;
    quat1.w          = w;
    quat1.x          = x;
    quat1.y          = y;
    Normalize(quat1);
}

//----------------------------------------------------------------------------------------

void RightMult(QuaternionInOut quat1, QuaternionIn quat2)
{
    const PFloat32 w = quat1.w * quat2.w - quat1.x * quat2.x - quat1.y * quat2.y - quat1.z * quat2.z;
    const PFloat32 x = quat1.w * quat2.x + quat1.x * quat2.w + quat1.y * quat2.z - quat1.z * quat2.y;
    const PFloat32 y = quat1.w * quat2.y - quat1.x * quat2.z + quat1.y * quat2.w + quat1.z * quat2.x;
    quat1.z          = quat1.w * quat2.z + quat1.x * quat2.y - quat1.y * quat2.x + quat1.z * quat2.w;
    quat1.w          = w;
    quat1.x          = x;
    quat1.y          = y;
    Normalize(quat1);
}

//----------------------------------------------------------------------------------------

void RotateVector(Vec3InOut v, QuaternionIn quat)
{
    // v = quat * (0,v.x,v.y,v.z) * ~quat
    Quaternion q( v.x * quat.x + v.y * quat.y + v.z * quat.z,
                  v.x * quat.w - v.y * quat.z + v.z * quat.y,
                  v.x * quat.z + v.y * quat.w - v.z * quat.x,
                 -v.x * quat.y + v.y * quat.x + v.z * quat.w);
    LeftMult(q, quat);
    v.x = q.x;
    v.y = q.y;
    v.z = q.z;
}

//----------------------------------------------------------------------------------------

Vec3Return RotateVector(Vec3In v, const Quaternion & quat)
{
    // v = quat * (0,v.x,v.y,v.z) * ~quat = quat * q
    Quaternion q( v.x * quat.x + v.y * quat.y + v.z * quat.z,
                  v.x * quat.w - v.y * quat.z + v.z * quat.y,
                  v.x * quat.z + v.y * quat.w - v.z * quat.x,
                 -v.x * quat.y + v.y * quat.x + v.z * quat.w);
    LeftMult(q, quat);
    return Vec3(q.x, q.y, q.z);
}

//----------------------------------------------------------------------------------------

void InverseRotateVector(Vec3InOut v, const Quaternion & quat)
{
    // v = ~quat * (0,v.x,v.y,v.z) * quat = q * quat
    Quaternion q(quat.x * v.x + quat.y * v.y + quat.z * v.z,
                 quat.w * v.x - quat.y * v.z + quat.z * v.y,
                 quat.w * v.y + quat.x * v.z - quat.z * v.x,
                 quat.w * v.z - quat.x * v.y + quat.y * v.x);
    RightMult(q, quat);
    v.x = q.x;
    v.y = q.y;
    v.z = q.z;
}

//----------------------------------------------------------------------------------------

Vec3Return InverseRotateVector(Vec3In v, const Quaternion & quat)
{
    // v = ~quat * (0,v.x,v.y,v.z) * quat = q * quat
    Quaternion q(quat.x * v.x + quat.y * v.y + quat.z * v.z,
                 quat.w * v.x - quat.y * v.z + quat.z * v.y,
                 quat.w * v.y + quat.x * v.z - quat.z * v.x,
                 quat.w * v.z - quat.x * v.y + quat.y * v.x);
    RightMult(q, quat);
    return Vec3(q.x, q.y, q.z);
}


}   // namespace Math
}   // namespace Pegasus
