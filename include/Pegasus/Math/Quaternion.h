/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Quaternion.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Quaternion types and operations

#ifndef PEGASUS_MATH_QUATERNION_H
#define PEGASUS_MATH_QUATERNION_H

#include "Pegasus/Math/Math.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"

namespace Pegasus {
namespace Math {


//! Quaternion structure
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Quaternion
{
    PFloat32 q[4];                        //!< Array version of the quaternion
    struct { PFloat32 w, x, y, z; };    //!< Structure version of the quaternion
                                        //!< (w : real component,
                                        //!<  x, y, z : imaginary components)

    Quaternion()                                    //!< Default constructor
        :    w(1.0f), x(1.0f), y(1.0f), z(1.0f) { };

    Quaternion(const Quaternion & q)            //!< Copy constructor
        :    w(q.w), x(q.x), y(q.y), z(q.z) { };

    Quaternion(const PFloat32 q[4])                    //!< Constructor with an array
        :    w(q[0]), x(q[1]), y(q[2]), z(q[3]) { };

    Quaternion(PFloat32 w, PFloat32 x, PFloat32 y, PFloat32 z)    //!< Constructor
        :    w(w), x(x), y(y), z(z) { };                                //!< with scalars

    Quaternion(Vec3In axis, PFloat32 angle);    //!< Constructor with
                                                        //!< axis-angle representation

    Quaternion(Mat33In mat);                //!< Constructor with rotation
                                                        //!< matrix representation

    Quaternion(Mat44In mat);                //!< Constructor with rotation
                                                        //!< matrix representation
};

//----------------------------------------------------------------------------------------

//! Function input type for a quaternion
typedef const Quaternion & QuaternionIn;

//! Function input/output type for a quaternion
typedef       Quaternion & QuaternionInOut;

//! Function return type for a quaternion
typedef       Quaternion   QuaternionReturn;

//----------------------------------------------------------------------------------------

// Creation and conversion functions

//! Modify the quaternion to be a default one
//! \param quat The modified quaternion (replaced by (1,0,0,0))
inline void SetDefaultQuaternion(QuaternionInOut quat) { quat.w = 1.0f; quat.x = 0.0f; quat.y = 0.0f; quat.z = 0.0f; }

//! Convert an axis with an angle to a quaternion
//! \param quat Destination quaternion
//! \param axis Axis of rotation
//! \param angle Angle of rotation (in radians)
void AxisAngleToQuaternion(QuaternionInOut quat, Vec3In axis, PFloat32 angle);

//! Convert an axis with an angle to a quaternion
//! \param axis Axis of rotation
//! \param angle Angle of rotation (in radians)
//! \return Destination quaternion
QuaternionReturn AxisAngleToQuaternion(Vec3In axis, PFloat32 angle);

//! Convert a quaternion to an axis with an angle
//! \param axis Resulting axis of rotation
//! \param angle Resulting angle of rotation (in radians)
//! \param quat Quaternion to convert
void QuaternionToAxisAngle(Vec3InOut axis, PFloat32 & angle, QuaternionIn quat);

//! Convert a quaternion to a 3x3 matrix
//! \param mat Resulting matrix
//! \param quat Quaternion to convert
void QuaternionToMat33(Mat33InOut mat, QuaternionIn quat);

//! Convert a quaternion to a transposed 3x3 matrix (for OpenGL for example)
//! \param mat Resulting matrix
//! \param quat Quaternion to convert
void QuaternionToMat33T(Mat33InOut mat, QuaternionIn quat);

//! Convert a quaternion to a 4x4 matrix
//! \param mat Resulting matrix
//! \param quat Quaternion to convert
void QuaternionToMat44(Mat44InOut mat, QuaternionIn quat);

//! Convert a quaternion to a transposed 4x4 matrix (for OpenGL for example)
//! \param mat Resulting matrix
//! \param quat Quaternion to convert
void QuaternionToMat44T(Mat44InOut mat, QuaternionIn quat);

//! Convert a 3x3 matrix to a quaternion
//! \param quat Resulting quaternion
//! \param mat Matrix to convert
void Mat33ToQuaternion(QuaternionInOut quat, Mat33In mat);

//! Convert a transposed 3x3 matrix to a quaternion
//! \param quat Resulting quaternion
//! \param mat Matrix to convert
void Mat33TToQuaternion(QuaternionInOut quat, Mat33In mat);

//! Convert a 4x4 matrix to a quaternion
//! \param quat Resulting quaternion
//! \param mat Matrix to convert
void Mat44ToQuaternion(QuaternionInOut quat, Mat44In mat);

//! Convert a transposed 4x4 matrix to a quaternion
//! \param quat Resulting quaternion
//! \param mat Matrix to convert
void Mat44TToQuaternion(QuaternionInOut quat, Mat44In mat);

//----------------------------------------------------------------------------------------

// Shortcuts for the creation of matrices from an axis and an angle

//! Set a 3x3 matrix to a rotation matrix around the input axis
//! \param mat Resulting rotation matrix
//! \param axis Axis of rotation
//! \param angle Rotation angle (in radians)
inline void SetRotation(Mat33InOut mat, Vec3In axis, PFloat32 angle)
    {
        Quaternion quat;
        AxisAngleToQuaternion(quat, axis, angle);
        QuaternionToMat33(mat, quat);
    }

//! Set a transposed 3x3 matrix to a rotation matrix around the input axis
//! \param mat Resulting rotation matrix
//! \param axis Axis of rotation
//! \param angle Rotation angle (in radians)
inline void SetRotationT(Mat33InOut mat, Vec3In axis, PFloat32 angle)
    {
        Quaternion quat;
        AxisAngleToQuaternion(quat, axis, angle);
        QuaternionToMat33T(mat, quat);
    }

//! Set a 4x4 matrix to a rotation matrix around the input axis
//! \param mat Resulting rotation matrix
//! \param axis Axis of rotation
//! \param angle Rotation angle (in radians)
inline void SetRotation(Mat44InOut mat, Vec3In axis, PFloat32 angle)
    {
        Quaternion quat;
        AxisAngleToQuaternion(quat, axis, angle);
        QuaternionToMat44(mat, quat);
    }

//! Set a transposed 4x4 matrix to a rotation matrix around the input axis
//! \param mat Resulting rotation matrix
//! \param axis Axis of rotation
//! \param angle Rotation angle (in radians)
inline void SetRotationT(Mat44InOut mat, Vec3In axis, PFloat32 angle)
    {
        Quaternion quat;
        AxisAngleToQuaternion(quat, axis, angle);
        QuaternionToMat44T(mat, quat);
    }

//----------------------------------------------------------------------------------------

// Unary operators

//! Return the conjugate of a quaternion
//! \param quat Source quaternion
inline QuaternionReturn Conjugate(QuaternionIn quat) { return Quaternion(quat.w, -quat.x, -quat.y, -quat.z); }

//! Return the conjugate of a quaternion
//! \param quat Source quaternion
inline QuaternionReturn operator ~ (QuaternionIn quat) { return Quaternion(quat.w, -quat.x, -quat.y, -quat.z); }

//! Return the opposite of a quaternion
//! \param quat Source quaternion
inline QuaternionReturn operator - (QuaternionIn quat) { return Quaternion(-quat.w, -quat.x, -quat.y, -quat.z); }

//! Add a quaternion to another one
//! \param quat1 First quaternion (which contains the result)
//! \param quat2 Second quaternion
inline void operator += (QuaternionInOut quat1, QuaternionIn quat2)
    {
        quat1.w += quat2.w;
        quat1.x += quat2.x;
        quat1.y += quat2.y;
        quat1.z += quat2.z;
    }

//! Subtract a quaternion from another one
//! \param quat1 First quaternion (which contains the result)
//! \param quat2 Second quaternion
inline void operator -= (QuaternionInOut quat1, QuaternionIn quat2)
    {
        quat1.w -= quat2.w;
        quat1.x -= quat2.x;
        quat1.y -= quat2.y;
        quat1.z -= quat2.z;
    }

//! Multiply a quaternion by a constant
//! \param quat The quaternion to multiply (which contains the result)
//! \param cst The constant
inline void operator *= (QuaternionInOut quat, PFloat32 cst)
    {
        quat.w *= cst;
        quat.x *= cst;
        quat.y *= cst;
        quat.z *= cst;
    }

//! Left multiply a quaternion by another one (quat1 = quat2 * quat1)
//! \param quat1 The quaternion to multiply (which contains the result)
//! \param quat2 The second quaternion
void LeftMult(QuaternionInOut quat1, QuaternionIn quat2);

//! Right multiply a quaternion by another one (quat1 = quat1 * quat2)
//! \param quat1 The quaternion to multiply (which contains the result)
//! \param quat2 The second quaternion
void RightMult(QuaternionInOut quat1, QuaternionIn quat2);

//! Return the norm of a quaternion
//! \param quat Source quaternion
//! \return Norm of the quaternion
inline PFloat32 Norm(QuaternionIn quat) { return Sqrt(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z); }

//! Return the reciprocal of the norm of a quaternion
//! \param quat Source quaternion
//! \return Reciprocal of the norm of the quaternion
inline PFloat32 RcpNorm(QuaternionIn quat) { return RcpSqrt(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z); }

//! Normalize a quaternion
//! \param quat The quaternion to normalize
inline void Normalize(QuaternionInOut quat) { quat *= RcpNorm(quat); }

//----------------------------------------------------------------------------------------

// Vector rotation functions

//! Rotation of a vector
//! \param v Rotated vector
//! \param quat Quaternion describing the rotation
void RotateVector(Vec3InOut v, QuaternionIn quat);

//! Rotation of a vector
//! \param v Vector to rotate
//! \param quat Quaternion describing the rotation
//! \return Rotated vector
Vec3Return RotateVector(Vec3In v, QuaternionIn quat);

//! Inverse rotation of a vector
//! \param v Rotated vector
//! \param quat Quaternion describing the rotation
void InverseRotateVector(Vec3InOut v, QuaternionIn quat);

//! Inverse rotation of a vector
//! \param v Vector to rotate
//! \param quat Quaternion describing the rotation
//! \return Rotated vector
Vec3Return InverseRotateVector(Vec3In v, QuaternionIn quat);


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_QUATERNION_H
