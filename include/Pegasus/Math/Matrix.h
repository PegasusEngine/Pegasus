/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Matrix.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Matrix types and operations (with other matrices and vectors)

#ifndef PEGASUS_MATH_MATRIX_H
#define PEGASUS_MATH_MATRIX_H

#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Math {


//! 2 by 2 matrix
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Mat22
{
    float m[4];                                //!< Array version of the vector
    struct { PFloat32 m11, m12;
             PFloat32 m21, m22;            };    //!< Single component access

    Mat22()                                //!< Default constructor (identity matrix)
        : m11(1.0f), m12(0.0f), m21(0.0f), m22(1.0f) { };

    Mat22(const Mat22 & m)            //!< Copy constructor
        : m11(m.m11), m12(m.m12), m21(m.m21), m22(m.m22) { };

    Mat22(const PFloat32 m[4])            //!< Constructor with an array (row-major)
        : m11(m[0]), m12(m[1]), m21(m[2]), m22(m[3]) { };

    Mat22(const Vec2 & v1, const Vec2 & v2) //!< Constructor with vectors (columns)
        : m11(v1.x), m12(v2.x), m21(v1.y), m22(v2.y) { };

    Mat22(PFloat32 m11, PFloat32 m12,            //!< Constructor with scalars
          PFloat32 m21, PFloat32 m22)
        : m11(m11), m12(m12), m21(m21), m22(m22) { };

    Mat22(PFloat32 s)                    //!< Constructor with one scalar (identity matrix multiplied by this scalar)
        : m11(s), m12(0.0f), m21(0.0f), m22(s) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 2 by 2 matrix
typedef const Mat22 &  Mat22In;

//! Function input/output type for a 2 by 2 matrix
typedef       Mat22 &  Mat22InOut;

//! Function return type for a 2 by 2 matrix
typedef       Mat22    Mat22Return;

//----------------------------------------------------------------------------------------

//! 3 by 3 matrix
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Mat33
{
    PFloat32 m[9];                            //!< Array version of the vector
    struct { PFloat32 m11, m12, m13;
             PFloat32 m21, m22, m23;
             PFloat32 m31, m32, m33;    };    //!< Single component access

    Mat33()                                //!< Default constructor (identity matrix)
        : m11(1.0f), m12(0.0f), m13(0.0f),
          m21(0.0f), m22(1.0f), m23(0.0f),
          m31(0.0f), m32(0.0f), m33(1.0f) { };

    Mat33(const Mat33 & m)            //!< Copy constructor
        : m11(m.m11), m12(m.m12), m13(m.m13),
          m21(m.m21), m22(m.m22), m23(m.m23),
          m31(m.m31), m32(m.m32), m33(m.m33) { };

    Mat33(const PFloat32 m[9])            //!< Constructor with an array (row-major)
        : m11(m[0]), m12(m[1]), m13(m[2]),
          m21(m[3]), m22(m[4]), m23(m[5]),
          m31(m[6]), m32(m[7]), m33(m[8]) { };

    Mat33(const Vec3 & v1,            //!< Constructor with vectors (columns)
          const Vec3 & v2,
          const Vec3 & v3)
        : m11(v1.x), m12(v2.x), m13(v3.x),
          m21(v1.y), m22(v2.y), m23(v3.y),
          m31(v1.z), m32(v2.z), m33(v3.z) { };

    Mat33(PFloat32 m11, PFloat32 m12, PFloat32 m13,    //!< Constructor with scalars
          PFloat32 m21, PFloat32 m22, PFloat32 m23,
          PFloat32 m31, PFloat32 m32, PFloat32 m33)
        : m11(m11), m12(m12), m13(m13),
          m21(m21), m22(m22), m23(m23),
          m31(m31), m32(m32), m33(m33) { };

    Mat33(PFloat32 s)                    //!< Constructor with one scalar (identity
                                            //!< matrix multiplied by this scalar)
        : m11(s),    m12(0.0f), m13(0.0f),
          m21(0.0f), m22(s),    m23(0.0f),
          m31(0.0f), m32(0.0f), m33(s)     { };

    Mat33(Mat22In m)            //!< Constructor that copies the upper left
                                            //!< corner and fills with the identity matrix
        : m11(m.m11), m12(m.m12), m13(0.0f),
          m21(m.m21), m22(m.m22), m23(0.0f),
          m31(0.0f ), m32(0.0f ), m33(1.0f) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 3 by 3 matrix
typedef const Mat33 &  Mat33In;

//! Function input/output type for a 3 by 3 matrix
typedef       Mat33 &  Mat33InOut;

//! Function return type for a 3 by 3 matrix
typedef       Mat33    Mat33Return;

//----------------------------------------------------------------------------------------

//! 4 by 4 matrix
union
#    ifdef _USE_INTEL_COMPILER
    __declspec(align(16))
#    endif
Mat44
{
    PFloat32 m[16];                                //!< Array version of the vector
    struct { PFloat32 m11, m12, m13, m14;
             PFloat32 m21, m22, m23, m24;
             PFloat32 m31, m32, m33, m34;
             PFloat32 m41, m42, m43, m44;    };    //!< Single component access

    Mat44()                                //!< Default constructor (identity matrix)
        : m11(1.0f), m12(0.0f), m13(0.0f), m14(0.0f),
          m21(0.0f), m22(1.0f), m23(0.0f), m24(0.0f),
          m31(0.0f), m32(0.0f), m33(1.0f), m34(0.0f),
          m41(0.0f), m42(0.0f), m43(0.0f), m44(1.0f) { };

    Mat44(const Mat44 & m)            //!< Copy constructor
        : m11(m.m11), m12(m.m12), m13(m.m13), m14(m.m14),
          m21(m.m21), m22(m.m22), m23(m.m23), m24(m.m24),
          m31(m.m31), m32(m.m32), m33(m.m33), m34(m.m34),
          m41(m.m41), m42(m.m42), m43(m.m43), m44(m.m44) { };

    Mat44(const PFloat32 m[16])            //!< Constructor with an array (row-major)
        : m11(m[0 ]), m12(m[1 ]), m13(m[2 ]), m14(m[3 ]),
          m21(m[4 ]), m22(m[5 ]), m23(m[6 ]), m24(m[7 ]),
          m31(m[8 ]), m32(m[9 ]), m33(m[10]), m34(m[11]),
          m41(m[12]), m42(m[13]), m43(m[14]), m44(m[15]) { };

    Mat44(const Vec4 & v1,            //!< Constructor with vectors (columns)
          const Vec4 & v2,
          const Vec4 & v3,
          const Vec4 & v4)
        : m11(v1.x), m12(v2.x), m13(v3.x), m14(v4.x),
          m21(v1.y), m22(v2.y), m23(v3.y), m24(v4.y),
          m31(v1.z), m32(v2.z), m33(v3.z), m34(v4.z),
          m41(v1.w), m42(v2.w), m43(v3.w), m44(v4.w) { };

    Mat44(PFloat32 m11, PFloat32 m12, PFloat32 m13, PFloat32 m14,    //!< Constructor
          PFloat32 m21, PFloat32 m22, PFloat32 m23, PFloat32 m24,    //!< with scalars
          PFloat32 m31, PFloat32 m32, PFloat32 m33, PFloat32 m34,
          PFloat32 m41, PFloat32 m42, PFloat32 m43, PFloat32 m44)
        : m11(m11), m12(m12), m13(m13), m14(m14),
          m21(m21), m22(m22), m23(m23), m24(m24),
          m31(m31), m32(m32), m33(m33), m34(m34),
          m41(m41), m42(m42), m43(m43), m44(m44) { };

    Mat44(PFloat32 s)                    //!< Constructor with one scalar (identity
                                            //!< matrix multiplied by this scalar,
                                            //!< except for W, scaling matrix)
        : m11(s   ), m12(0.0f), m13(0.0f), m14(0.0f),
          m21(0.0f), m22(s),    m23(0.0f), m24(0.0f),
          m31(0.0f), m32(0.0f), m33(s)   , m34(0.0f),
          m41(0.0f), m42(0.0f), m43(0.0f), m44(1.0f) { };

    Mat44(Mat22In m)            //!< Constructor that copies the upper left
                                            //!< corner and fills with the identity matrix
        : m11(m.m11), m12(m.m12), m13(0.0f ), m14(0.0f),
          m21(m.m21), m22(m.m22), m23(0.0f ), m24(0.0f),
          m31(0.0f ), m32(0.0f ), m33(1.0f ), m34(0.0f),
          m41(0.0f ), m42(0.0f ), m43(0.0f ), m44(1.0f) { };

    Mat44(Mat33In m)            //!< Constructor that copies the upper left
                                            //!< corner and fills with the identity matrix
        : m11(m.m11), m12(m.m12), m13(m.m13), m14(0.0f),
          m21(m.m21), m22(m.m22), m23(m.m23), m24(0.0f),
          m31(m.m31), m32(m.m32), m33(m.m33), m34(0.0f),
          m41(0.0f ), m42(0.0f ), m43(0.0f ), m44(1.0f) { };
};

//----------------------------------------------------------------------------------------

//! Function input type for a 4 by 4 matrix
typedef const Mat44 &  Mat44In;

//! Function input/output type for a 4 by 4 matrix
typedef       Mat44 &  Mat44InOut;

//! Function return type for a 4 by 4 matrix
typedef       Mat44    Mat44Return;

//----------------------------------------------------------------------------------------

// Matrix constants

//! \todo Make those matrices part of the classes

//@{
//! Identity matrix
const Mat22 MAT22_IDENTITY(1.0f, 0.0f,
                           0.0f, 1.0f);

const Mat33 MAT33_IDENTITY(1.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 1.0f);

const Mat44 MAT44_IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f);
//@}

//----------------------------------------------------------------------------------------

//! Convert a 3x3 matrix to 4x4. The last lines and columns are (0,0,0,1)
//! \param dst Destination 4x4 matrix
//! \param src Source 3x3 matrix
void Mat44FromMat33(Mat44InOut dst, Mat33In src);

//! Convert a 4x4 matrix to 3x3. Only the 3x3 top left corner of the input matrix is kept
//! \param dst Destination 3x3 matrix
//! \param src Source 4x4 matrix
void Mat33FromMat44(Mat33InOut dst, Mat44In src);

//----------------------------------------------------------------------------------------

//@{
//! Set a matrix to identity
//! \param mat Matrix to set to identity
void SetIdentity(Mat22InOut mat);
void SetIdentity(Mat33InOut mat);
void SetIdentity(Mat44InOut mat);
//@}

//----------------------------------------------------------------------------------------


//@{
//! Transpose a matrix
//! \param mat Matrix to transpose
void Transpose(Mat22InOut mat);
void Transpose(Mat33InOut mat);
void Transpose(Mat44InOut mat);
//@}

//@{
//! Transpose of a matrix
//! \param trans Resulting transposed matrix
//! \param mat Matrix to transpose
void Transpose(Mat22InOut trans, Mat22In mat);
void Transpose(Mat33InOut trans, Mat33In mat);
void Transpose(Mat44InOut trans, Mat44In mat);
//@}

//----------------------------------------------------------------------------------------

//! \todo Make the FORCE_GENERIC flag available in preprocessor.h
#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

// Multiplication of a matrix and a vector, and the result stored in an other vector
/*typedef void (* Mult22_21)(Vec2InOut dst, Mat22In mat, Vec2In vec);
typedef void (* Mult33_31)(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/typedef void (* Mult44_41)(Vec4InOut dst, Mat44In mat, Vec4In vec);
/*extern Mult22_21 Mult22_21;
extern Mult33_31 Mult33_31;
*/extern Mult44_41 Mult44_41;

// Multiplication of a transposed matrix and a vector,
// and the result stored in an other vector
/*typedef void (* Mult22T_21)(Vec2InOut dst, Mat22In mat, Vec2In vec);
typedef void (* Mult33T_31)(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/typedef void (* Mult44T_41)(Vec4InOut dst, Mat44In mat, Vec4In vec);
/*extern Mult22T_21 Mult22T_21;
extern Mult33T_31 Mult33T_31;
*/extern Mult44T_41 Mult44T_41;

inline void Mult22_21(Vec2InOut dst, Mat22In mat, Vec2In vec);
inline void Mult33_31(Vec3InOut dst, Mat33In mat, Vec3In vec);
inline void Mult22T_21(Vec2InOut dst, Mat22In mat, Vec2In vec);
inline void Mult33T_31(Vec3InOut dst, Mat33In mat, Vec3In vec);

#else

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! \param mat The 2x2 matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The resulting 2D vector
void Mult22_21(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! \param mat The 3x3 matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The resulting 3D vector
void Mult33_31(Vec3InOut dst, Mat33In mat, Vec3In vec);

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! \param mat The 4x4 matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The resulting 4D vector
void Mult44_41(Vec4InOut dst, Mat44In mat, Vec4In vec);

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector
//! \param mat The 2x2 transposed matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The resulting 2D vector
void Mult22T_21(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector
//! \param mat The 3x3 transposed matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The resulting 3D vector
void Mult33T_31(Vec3InOut dst, Mat33In mat, Vec3In vec);

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector
//! \param mat The 4x4 transposed matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The resulting 4D vector
void Mult44T_41(Vec4InOut dst, Mat44In mat, Vec4In vec);

//! Multiplication between two matrices, and the result stored in an other matrix
//! \param mat1 The first 2x2 matrix for the multiplication
//! \param mat2 The second 2x2 matrix for the multiplication
//! \param dst The resulting 2x2 matrix (= mat1 * mat2)
void Mult22_22(Mat22InOut dst, Mat22In mat1, Mat22In mat2);

//! Multiplication between two matrices, and the result stored in an other matrix
//! \param mat1 The first 3x3 matrix for the multiplication
//! \param mat2 The second 3x3 matrix for the multiplication
//! \param dst The resulting 3x3 matrix (= mat1 * mat2)
void Mult33_33(Mat33InOut dst, Mat33In mat1, Mat33In mat2);

//! Multiplication between two matrices, and the result stored in an other matrix
//! \param mat1 The first 4x4 matrix for the multiplication
//! \param mat2 The second 4x4 matrix for the multiplication
//! \param dst The resulting 4x4 matrix (= mat1 * mat2)
void Mult44_44(Mat44InOut dst, Mat44In mat1, Mat44In mat2);

#endif    // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

//----------------------------------------------------------------------------------------

//@{
//! Determinant of a matrix
//! \param mat The input matrix
//! \return Determinant of the matrix
PFloat32 Determinant(Mat22In mat);
PFloat32 Determinant(Mat33In mat);
PFloat32 Determinant(Mat44In mat);
//@}

//----------------------------------------------------------------------------------------

//! Inverse of a 3x3 matrix
//! \param mat The input 3x3 matrix
//! \param dst The resulting inversed 3x3 matrix
//void Inverse(Mat33InOut dst, Mat33In mat);

//! Inverse of a 4x4 matrix
//! \param mat The input 4x4 matrix
//! \param dst The resulting inversed 4x4 matrix
//void Inverse(Mat44InOut dst, Mat44In mat);

//! Inverse of a homogeneous 4x4 matrix (containing a rotation in the top left 3x3 corner
//! and a translation in the last column)
//! \param mat The input 4x4 matrix (the last line must be (0,0,0,1)
//!            and the top left 3x3 corner must be orthogonal)
void InverseHomogeneous(Mat44InOut dst, Mat44In mat);

//----------------------------------------------------------------------------------------

//! Solve a linear system of 3 equations of 3 unknowns (A.X = B)
//! \warning The A matrix must have a non-zero determinant (assertion error otherwise)
//! \note Uses the determinant method (Cramer)
//! \param A Matrix of coefficients (each row represents an equation)
//! \param B Vector of constants
//! \return Resulting X vector
Vec3Return SolveLinearSystem(Mat33In A, Vec3In B);

//----------------------------------------------------------------------------------------

// 3x3 versions of rotation matrices

//! Set a 3x3 matrix to a rotation matrix around the X axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationX(Mat33InOut mat, PFloat32 angle);

//! Set a 3x3 matrix to a transposed rotation matrix around the X axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationXT(Mat33InOut mat, PFloat32 angle);

//! Set a 3x3 matrix to a rotation matrix around the Y axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationY(Mat33InOut mat, PFloat32 angle);

//! Set a 3x3 matrix to a transposed rotation matrix around the Y axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationYT(Mat33InOut mat, PFloat32 angle);

//! Set a 3x3 matrix to a rotation matrix around the Z axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationZ(Mat33InOut mat, PFloat32 angle);

//! Set a 3x3 matrix to a transposed rotation matrix around the Z axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationZT(Mat33InOut mat, PFloat32 angle);

//----------------------------------------------------------------------------------------

// 4x4 versions of transformation matrices

//! Set a 4x4 matrix to a translation matrix
//! \param mat Resulting translation matrix
//! \param translation Translation vector
void SetTranslation(Mat44InOut mat, Vec3In translation);

//! Set a 4x4 matrix to a transposed translation matrix
//! \param mat Resulting transposed translation matrix
//! \param translation Translation vector
void SetTranslationT(Mat44InOut mat, Vec3In translation);

//! Set a 4x4 matrix to a rotation matrix around the X axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationX(Mat44InOut mat, PFloat32 angle);

//! Set a 4x4 matrix to a transposed rotation matrix around the X axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationXT(Mat44InOut mat, PFloat32 angle);

//! Set a 4x4 matrix to a rotation matrix around the Y axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationY(Mat44InOut mat, PFloat32 angle);

//! Set a 4x4 matrix to a transposed rotation matrix around the Y axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationYT(Mat44InOut mat, PFloat32 angle);

//! Set a 4x4 matrix to a rotation matrix around the Z axis
//! \param mat Resulting rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationZ(Mat44InOut mat, PFloat32 angle);

//! Set a 4x4 matrix to a transposed rotation matrix around the Z axis
//! \param mat Resulting transposed rotation matrix
//! \param angle Rotation angle (in radians)
void SetRotationZT(Mat44InOut mat, PFloat32 angle);

// For the creation of a matrix from an axis and an angle,
// go to Quaternion.h

//----------------------------------------------------------------------------------------

//@{
//! Write the content of a matrix to the debug outputs for debugging
//! \param mat Matrix to debug
//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(Mat22In mat);
//void WriteMatToDebugOutputs(Mat33In mat);
//void WriteMatToDebugOutputs(Mat44In mat);
//@}

//! Write the content of a matrix to the debug outputs for debugging
//! \param mat Matrix to debug in the format of a 2D array
//! \param nRows Number of rows of the matrix
//! \param nColumns Number of columns of the matrix
//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(PFloat32 ** mat, PUInt32 nRows, PUInt32 nColumns);


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_MATRIX_H
