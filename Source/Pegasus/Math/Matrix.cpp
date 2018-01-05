/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Matrix.cpp
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Matrix types and operations (with other matrices and vectors)

#include "Pegasus/Math/Matrix.h"
//#include "Core/DebugOutputs.h"

namespace Pegasus {
namespace Math {


void Mat44FromMat33(Mat44InOut dst, Mat33In src)
{
    dst.m11 = src.m11;  dst.m12 = src.m12;  dst.m13 = src.m13;  dst.m14 = 0.0f;
    dst.m21 = src.m21;  dst.m22 = src.m22;  dst.m23 = src.m23;  dst.m24 = 0.0f;
    dst.m31 = src.m31;  dst.m32 = src.m32;  dst.m33 = src.m33;  dst.m34 = 0.0f;
    dst.m41 = 0.0f;     dst.m42 = 0.0f;     dst.m43 = 0.0f;     dst.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void Mat33FromMat44(Mat33InOut dst, Mat44In src)
{
    dst.m11 = src.m11;  dst.m12 = src.m12;  dst.m13 = src.m13;
    dst.m21 = src.m21;  dst.m22 = src.m22;  dst.m23 = src.m23;
    dst.m31 = src.m31;  dst.m32 = src.m32;  dst.m33 = src.m33;
}

//----------------------------------------------------------------------------------------

void SetIdentity(Mat22InOut mat)
{
    mat.m11 = 1.0f;     mat.m12 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;
}

void SetIdentity(Mat33InOut mat)
{
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;
}

void SetIdentity(Mat44InOut mat)
{
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;     mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void Transpose(Mat22InOut mat)
{
    const PFloat32 t = mat.m12;
    mat.m12 = mat.m21;
    mat.m21 = t;
}

void Transpose(Mat33InOut mat)
{
    PFloat32 t = mat.m12;
    mat.m12 = mat.m21;
    mat.m21 = t;

    t = mat.m13;
    mat.m13 = mat.m31;
    mat.m31 = t;

    t = mat.m23;
    mat.m23 = mat.m32;
    mat.m32 = t;
}

void Transpose(Mat44InOut mat)
{
    PFloat32 t = mat.m12;
    mat.m12 = mat.m21;
    mat.m21 = t;

    t = mat.m13;
    mat.m13 = mat.m31;
    mat.m31 = t;

    t = mat.m14;
    mat.m14 = mat.m41;
    mat.m41 = t;

    t = mat.m23;
    mat.m23 = mat.m32;
    mat.m32 = t;

    t = mat.m24;
    mat.m24 = mat.m42;
    mat.m42 = t;

    t = mat.m34;
    mat.m34 = mat.m43;
    mat.m43 = t;
}

//----------------------------------------------------------------------------------------

void Transpose(Mat22InOut trans, Mat22In mat)
{
    //! \todo Manage in place transposition
    trans.m11 = mat.m11;    trans.m12 = mat.m21;
    trans.m21 = mat.m12;    trans.m22 = mat.m22;
}

void Transpose(Mat33InOut trans, Mat33In mat)
{
    //! \todo Manage in place transposition
    trans.m11 = mat.m11;    trans.m12 = mat.m21;    trans.m13 = mat.m31;
    trans.m21 = mat.m12;    trans.m22 = mat.m22;    trans.m23 = mat.m32;
    trans.m31 = mat.m13;    trans.m32 = mat.m23;    trans.m33 = mat.m33;
}

void Transpose(Mat44InOut trans, Mat44In mat)
{
    //! \todo Manage in place transposition
    trans.m11 = mat.m11;    trans.m12 = mat.m21;    trans.m13 = mat.m31;    trans.m14 = mat.m41;
    trans.m21 = mat.m12;    trans.m22 = mat.m22;    trans.m23 = mat.m32;    trans.m24 = mat.m42;
    trans.m31 = mat.m13;    trans.m32 = mat.m23;    trans.m33 = mat.m33;    trans.m34 = mat.m43;
    trans.m41 = mat.m14;    trans.m42 = mat.m24;    trans.m43 = mat.m34;    trans.m44 = mat.m44;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

// Function pointers (initialized in function of the available hardware)

// Multiplication between a matrix and a vector, and the result stored in an other vector
/*Mult22_21 Mult22_21 = 0;
Mult33_31 Mult33_31 = 0;*/
Mult44_41 Mult44_41 = 0;

// Multiplication between a transposed matrix and a vector,
// and the result stored in an other vector
/*Mult22_21 Mult22T_21 = 0;
Mult33_31 Mult33T_31 = 0;*/
Mult44_41 Mult44T_41 = 0;

/*
#else

// Generic implementation

//----------------------------------------------------------------------------------------

void Mult22_21(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y;
    dst.y       = mat.m21 * vec.x + mat.m22 * vec.y;
    dst.x       = t0;
}

void Mult33_31(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y + mat.m13 * vec.z;
    PFloat32 t1 = mat.m21 * vec.x + mat.m22 * vec.y + mat.m23 * vec.z;
    dst.z       = mat.m31 * vec.x + mat.m32 * vec.y + mat.m33 * vec.z;
    dst.x       = t0;
    dst.y       = t1;
}

void Mult44_41(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y + mat.m13 * vec.z + mat.m14 * vec.w;
    PFloat32 t1 = mat.m21 * vec.x + mat.m22 * vec.y + mat.m23 * vec.z + mat.m24 * vec.w;
    PFloat32 t2 = mat.m31 * vec.x + mat.m32 * vec.y + mat.m33 * vec.z + mat.m34 * vec.w;
    dst.w       = mat.m41 * vec.x + mat.m42 * vec.y + mat.m43 * vec.z + mat.m44 * vec.w;
    dst.x       = t0;
    dst.y       = t1;
    dst.z       = t2;
}

//----------------------------------------------------------------------------------------

void Mult22T_21(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    PFloat32 t0 =    mat.m11 * vec.x + mat.m21 * vec.y;
    dst.y  =    mat.m12 * vec.x + mat.m22 * vec.y;
    dst.x  = t0;
}

void Mult33T_31(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    PFloat32 t0 = mat.m11 * vec.x + mat.m21 * vec.y + mat.m31 * vec.z;
    PFloat32 t1 = mat.m12 * vec.x + mat.m22 * vec.y + mat.m32 * vec.z;
    dst.z       = mat.m13 * vec.x + mat.m23 * vec.y + mat.m33 * vec.z;
    dst.x       = t0;
    dst.y       = t1;
}

void Mult44T_41(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    PFloat32 t0 = mat.m11 * vec.x + mat.m21 * vec.y + mat.m31 * vec.z + mat.m41 * vec.w;
    PFloat32 t1 = mat.m12 * vec.x + mat.m22 * vec.y + mat.m32 * vec.z + mat.m42 * vec.w;
    PFloat32 t2 = mat.m13 * vec.x + mat.m23 * vec.y + mat.m33 * vec.z + mat.m43 * vec.w;
    dst.w       = mat.m14 * vec.x + mat.m24 * vec.y + mat.m34 * vec.z + mat.m44 * vec.w;
    dst.x       = t0;
    dst.y       = t1;
    dst.z       = t2;
}
*/

#endif    // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

//----------------------------------------------------------------------------------------

/*#if !PEGASUS_ARCH_IA32 || PEGASUS_MATH_FORCE_GENERIC*/

void Mult22_21(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    const PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y;
    dst.y             = mat.m21 * vec.x + mat.m22 * vec.y;
    dst.x             = t0;
}

void Mult33_31(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    const PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y + mat.m13 * vec.z;
    const PFloat32 t1 = mat.m21 * vec.x + mat.m22 * vec.y + mat.m23 * vec.z;
    dst.z             = mat.m31 * vec.x + mat.m32 * vec.y + mat.m33 * vec.z;
    dst.x             = t0;
    dst.y             = t1;
}

/*************/
#if !PEGASUS_ARCH_IA32 || PEGASUS_MATH_FORCE_GENERIC
/*************/

void Mult44_41(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    const PFloat32 t0 = mat.m11 * vec.x + mat.m12 * vec.y + mat.m13 * vec.z + mat.m14 * vec.w;
    const PFloat32 t1 = mat.m21 * vec.x + mat.m22 * vec.y + mat.m23 * vec.z + mat.m24 * vec.w;
    const PFloat32 t2 = mat.m31 * vec.x + mat.m32 * vec.y + mat.m33 * vec.z + mat.m34 * vec.w;
    dst.w             = mat.m41 * vec.x + mat.m42 * vec.y + mat.m43 * vec.z + mat.m44 * vec.w;
    dst.x             = t0;
    dst.y             = t1;
    dst.z             = t2;
}

/****************/
#endif
/****************/

//----------------------------------------------------------------------------------------

void Mult22T_21(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    const PFloat32 t0 = mat.m11 * vec.x + mat.m21 * vec.y;
    dst.y             = mat.m12 * vec.x + mat.m22 * vec.y;
    dst.x             = t0;
}

void Mult33T_31(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    const PFloat32 t0 = mat.m11 * vec.x + mat.m21 * vec.y + mat.m31 * vec.z;
    const PFloat32 t1 = mat.m12 * vec.x + mat.m22 * vec.y + mat.m32 * vec.z;
    dst.z             = mat.m13 * vec.x + mat.m23 * vec.y + mat.m33 * vec.z;
    dst.x             = t0;
    dst.y             = t1;
}

/****************/
#if !PEGASUS_ARCH_IA32 || PEGASUS_MATH_FORCE_GENERIC
/****************/

void Mult44T_41(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    const PFloat32 t0 =  mat.m11 * vec.x + mat.m21 * vec.y + mat.m31 * vec.z + mat.m41 * vec.w;
    const PFloat32 t1 =  mat.m12 * vec.x + mat.m22 * vec.y + mat.m32 * vec.z + mat.m42 * vec.w;
    const PFloat32 t2 =  mat.m13 * vec.x + mat.m23 * vec.y + mat.m33 * vec.z + mat.m43 * vec.w;
    dst.w             = mat.m14 * vec.x + mat.m24 * vec.y + mat.m34 * vec.z + mat.m44 * vec.w;
    dst.x             = t0;
    dst.y             = t1;
    dst.z             = t2;
}

//----------------------------------------------------------------------------------------

void Mult22_22(Mat22InOut dst, Mat22In mat1, Mat22In mat2)
{
    const PFloat32 t11 = mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21;
    const PFloat32 t21 = mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21;
    const PFloat32 t12 = mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22;
    dst.m22            = mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22;
    dst.m12            = t12;
    dst.m21            = t21;
    dst.m11            = t11;
}

void Mult33_33(Mat33InOut dst, Mat33In mat1, Mat33In mat2)
{
    const PFloat32 t11 =  mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21 + mat1.m13 * mat2.m31;
    const PFloat32 t21 =  mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21 + mat1.m23 * mat2.m31;
    const PFloat32 t31 =  mat1.m31 * mat2.m11 + mat1.m32 * mat2.m21 + mat1.m33 * mat2.m31;

    const PFloat32 t12 =  mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22 + mat1.m13 * mat2.m32;
    const PFloat32 t22 =  mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22 + mat1.m23 * mat2.m32;
    const PFloat32 t32 =  mat1.m31 * mat2.m12 + mat1.m32 * mat2.m22 + mat1.m33 * mat2.m32;

    const PFloat32 t13 =  mat1.m11 * mat2.m13 + mat1.m12 * mat2.m23 + mat1.m13 * mat2.m33;
    const PFloat32 t23 =  mat1.m21 * mat2.m13 + mat1.m22 * mat2.m23 + mat1.m23 * mat2.m33;
    dst.m33            =  mat1.m31 * mat2.m13 + mat1.m32 * mat2.m23 + mat1.m33 * mat2.m33;
    dst.m23            = t23;
    dst.m13            = t13;
    dst.m32            = t32;
    dst.m22            = t22;
    dst.m12            = t12;
    dst.m31            = t31;
    dst.m21            = t21;
    dst.m11            = t11;
}

void Mult44_44(Mat44InOut dst, Mat44In mat1, Mat44In mat2)
{
    const PFloat32 t11 =  mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21 + mat1.m13 * mat2.m31 + mat1.m14 * mat2.m41;
    const PFloat32 t21 =  mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21 + mat1.m23 * mat2.m31 + mat1.m24 * mat2.m41;
    const PFloat32 t31 =  mat1.m31 * mat2.m11 + mat1.m32 * mat2.m21 + mat1.m33 * mat2.m31 + mat1.m34 * mat2.m41;
    const PFloat32 t41 =  mat1.m41 * mat2.m11 + mat1.m42 * mat2.m21 + mat1.m43 * mat2.m31 + mat1.m44 * mat2.m41;

    const PFloat32 t12 =  mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22 + mat1.m13 * mat2.m32 + mat1.m14 * mat2.m42;
    const PFloat32 t22 =  mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22 + mat1.m23 * mat2.m32 + mat1.m24 * mat2.m42;
    const PFloat32 t32 =  mat1.m31 * mat2.m12 + mat1.m32 * mat2.m22 + mat1.m33 * mat2.m32 + mat1.m34 * mat2.m42;
    const PFloat32 t42 =  mat1.m41 * mat2.m12 + mat1.m42 * mat2.m22 + mat1.m43 * mat2.m32 + mat1.m44 * mat2.m42;

    const PFloat32 t13 =  mat1.m11 * mat2.m13 + mat1.m12 * mat2.m23 + mat1.m13 * mat2.m33 + mat1.m14 * mat2.m43;
    const PFloat32 t23 =  mat1.m21 * mat2.m13 + mat1.m22 * mat2.m23 + mat1.m23 * mat2.m33 + mat1.m24 * mat2.m43;
    const PFloat32 t33 =  mat1.m31 * mat2.m13 + mat1.m32 * mat2.m23 + mat1.m33 * mat2.m33 + mat1.m34 * mat2.m43;
    const PFloat32 t43 =  mat1.m41 * mat2.m13 + mat1.m42 * mat2.m23 + mat1.m43 * mat2.m33 + mat1.m44 * mat2.m43;

    const PFloat32 t14 =  mat1.m11 * mat2.m14 + mat1.m12 * mat2.m24 + mat1.m13 * mat2.m34 + mat1.m14 * mat2.m44;
    const PFloat32 t24 =  mat1.m21 * mat2.m14 + mat1.m22 * mat2.m24 + mat1.m23 * mat2.m34 + mat1.m24 * mat2.m44;
    const PFloat32 t34 =  mat1.m31 * mat2.m14 + mat1.m32 * mat2.m24 + mat1.m33 * mat2.m34 + mat1.m34 * mat2.m44;
    dst.m44            =  mat1.m41 * mat2.m14 + mat1.m42 * mat2.m24 + mat1.m43 * mat2.m34 + mat1.m44 * mat2.m44;
    dst.m34            = t34;
    dst.m24            = t24;
    dst.m14            = t14;

    dst.m43            = t43;
    dst.m33            = t33;
    dst.m23            = t23;
    dst.m13            = t13;

    dst.m42            = t42;
    dst.m32            = t32;
    dst.m22            = t22;
    dst.m12            = t12;

    dst.m41            = t41;
    dst.m31            = t31;
    dst.m21            = t21;
    dst.m11            = t11;
}

/****************/
#endif    // !PEGASUS_ARCH_IA32 || PEGASUS_MATH_FORCE_GENERIC
/****************/

//----------------------------------------------------------------------------------------

PFloat32 Determinant(Mat22In mat)
{
    return mat.m11 * mat.m22 - mat.m21 * mat.m12;
}

PFloat32 Determinant(Mat33In mat)
{
    return   mat.m11 * (mat.m22 * mat.m33 - mat.m32 * mat.m23)
           + mat.m12 * (mat.m23 * mat.m31 - mat.m33 * mat.m21)
           + mat.m13 * (mat.m21 * mat.m32 - mat.m31 * mat.m22);
}

PFloat32 Determinant(Mat44In mat)
{
    //! \todo Implement the determinant of a 4x4 matrix
    /***********/
    //N3D_TODO
    /****/return 0.0f;
}

//----------------------------------------------------------------------------------------

void InverseHomogeneous(Mat44InOut dst, Mat44In mat)
{
    //! \todo Manage in place inversion

    PG_ASSERT(   (mat.m41 == 0.0f) && (mat.m42 == 0.0f)
              && (mat.m43 == 0.0f) && (mat.m44 == 1.0f) );

    // Transpose the rotation matrix
    dst.m11 = mat.m11;    dst.m12 = mat.m21;    dst.m13 = mat.m31;
    dst.m21 = mat.m12;    dst.m22 = mat.m22;    dst.m23 = mat.m32;
    dst.m31 = mat.m13;    dst.m32 = mat.m23;    dst.m33 = mat.m33;

    // Multiply the transposed rotation matrix by the translation vector
    // and take the opposite
    dst.m14 = - (mat.m11 * mat.m14 + mat.m21 * mat.m24 + mat.m31 * mat.m34);
    dst.m24 = - (mat.m12 * mat.m14 + mat.m22 * mat.m24 + mat.m32 * mat.m34);
    dst.m34 = - (mat.m13 * mat.m14 + mat.m23 * mat.m24 + mat.m33 * mat.m34);

    // Fill the last row
    dst.m41 = 0.0f;
    dst.m42 = 0.0f;
    dst.m43 = 0.0f;
    dst.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------
//-- generic template matrix multiplication
template <unsigned N, class T> void GenericAddRow(int row, T * r1, T * r2, int resultRows)
{
    for (int i = 0 ; i < N; ++i)
    {
        int srcIdx =  N*resultRows + i;
        int dstIdx =  N*row + i;
        r1[srcIdx] += r1[dstIdx]; 
        r2[srcIdx] += r2[dstIdx]; 
    }
}

template<unsigned N, class T> void GenericMulRow(T * r1, T * r2, int r, T f)
{
    for (int i = 0; i < N; ++i)
    {
        int dstIdx =  N*r + i;
        r1[dstIdx] *= f;
        r2[dstIdx] *= f;
    }
}

template<unsigned N, class T> void GenericSwapRow(T * M ,int i, int j)
{
    for (int k = 0; k < N; ++k)
    {
        int srcIndx = N*i + k;
        int dstIndx = N*j + k;
        T tmp = M[dstIndx];
        M[dstIndx] = M[srcIndx];
        M[srcIndx] = tmp;
    }
}

template <unsigned N, class T> void GenericInverse(T * M, T * result)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j) result[N*i + j] = i == j ? static_cast<T>(1) : static_cast<T>(0);
    }
    for (int i = 0; i < N; ++i)
    {
        //find max row
        for (int k = i+1; k < N; ++k)
        {
            if (Abs(M[N*i + i]) < Abs(M[N*k + i]))
            {
                GenericSwapRow<N,T>(M,i,k);
                GenericSwapRow<N,T>(result,i,k);
            } 
        }
        //zero out columns
        for (int j = i + 1; j < N; ++j)
        {
            T d = M[N*i + i];
            if (Abs(M[N*j + i]) < 0.00001f)continue;
            T factor = -d / M[N*j + i];
            GenericMulRow<N,T>(M,result,j,factor);
            GenericAddRow<N,T>(i,M,result,j);
        }
    }

    for (int i = 0; i < N; ++i)
    {
        for (int j = i + 1; j < N; ++j)
        {
            if (Abs(M[N*i + j]) < 0.00001f) continue;
            T factor = -M[N*i + j] / M[N*j + j];
            GenericMulRow<N,T>(M,result,j,factor);
            GenericAddRow<N,T>(j,M,result,i);
        } 
        GenericMulRow<N,T>(M,result,i,1.0f/M[N*i + i]);
    }
}

void Inverse(Mat44InOut dst, Mat44In mat)
{
    Mat44 tmp = mat;
    GenericInverse<4, PFloat32>(tmp.m,dst.m);
}


void Inverse(Mat33InOut dst, Mat33In mat)
{
    Mat33 tmp = mat;
    GenericInverse<3, PFloat32>(tmp.m,dst.m);
}

//----------------------------------------------------------------------------------------

Vec3Return SolveLinearSystem(Mat33In A, Vec3In B)
{
    // Compute the determinant of A
    const PFloat32 detA = Determinant(A);

    // Test if the determinant is not null
    PG_ASSERT(detA != 0.0f);

    // Compute the determinant of A where the first column is replaced by B
    const PFloat32 detA1B =  B.v[0] * (A.m22  * A.m33  - A.m32  * A.m23 )
                           + A.m12  * (A.m23  * B.v[2] - A.m33  * B.v[1])
                           + A.m13  * (B.v[1] * A.m32  - B.v[2] * A.m22 );

    // Compute the determinant of A where the second column is replaced by B
    const PFloat32 detA2B =  A.m11  * (B.v[1] * A.m33  - B.v[2] * A.m23 )
                           + B.v[0] * (A.m23  * A.m31  - A.m33  * A.m21 )
                           + A.m13  * (A.m21  * B.v[2] - A.m31  * B.v[1]);

    // Compute the determinant of A where the third column is replaced by B
    const PFloat32 detA3B =  A.m11  * (A.m22  * B.v[2] - A.m32  * B.v[1])
                           + A.m12  * (B.v[1] * A.m31  - B.v[2] * A.m21 )
                           + B.v[0] * (A.m21  * A.m32  - A.m31  * A.m22 );

    // Return the final solution
    const PFloat32 recDetA = 1.0f / detA;
    return Vec3(detA1B * recDetA, detA2B * recDetA, detA3B * recDetA);
}

//----------------------------------------------------------------------------------------

void SetRotationX(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = c;        mat.m23 = -s;
    mat.m31 = 0.0f;     mat.m32 = s;        mat.m33 = c;
}

//----------------------------------------------------------------------------------------

void SetRotationXT(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = c;        mat.m23 = s;
    mat.m31 = 0.0f;     mat.m32 = -s;       mat.m33 = c;
}

//----------------------------------------------------------------------------------------

void SetRotationY(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = 0.0f;     mat.m13 = s;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;
    mat.m31 = -s;       mat.m32 = 0.0f;     mat.m33 = c;
}

//----------------------------------------------------------------------------------------

void SetRotationYT(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = 0.0f;     mat.m13 = -s;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;
    mat.m31 = s;        mat.m32 = 0.0f;     mat.m33 = c;
}

//----------------------------------------------------------------------------------------

void SetRotationZ(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = -s;       mat.m13 = 0.0f;
    mat.m21 = s;        mat.m22 = c;        mat.m23 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationZT(Mat33InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = s;        mat.m13 = 0.0f;
    mat.m21 = -s;       mat.m22 = c;        mat.m23 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetTranslation(Mat44InOut mat, Vec3In translation)
{
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;     mat.m14 = translation.x;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;     mat.m24 = translation.y;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;     mat.m34 = translation.z;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetTranslationT(Mat44InOut mat, Vec3In translation)
{
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;     mat.m34 = 0.0f;
    mat.m41 = translation.x;                mat.m42 = translation.y;
    mat.m43 = translation.z;                mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationX(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = c;        mat.m23 = -s;       mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = s;        mat.m33 = c;        mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationXT(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = 1.0f;     mat.m12 = 0.0f;     mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = c;        mat.m23 = s;        mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = -s;       mat.m33 = c;        mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationY(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = 0.0f;     mat.m13 = s;        mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = -s;       mat.m32 = 0.0f;     mat.m33 = c;        mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationYT(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = 0.0f;     mat.m13 = -s;       mat.m14 = 0.0f;
    mat.m21 = 0.0f;     mat.m22 = 1.0f;     mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = s;        mat.m32 = 0.0f;     mat.m33 = c;        mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationZ(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = -s;       mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = s;        mat.m22 = c;        mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;     mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetRotationZT(Mat44InOut mat, PFloat32 angle)
{
    const PFloat32 c = Cos(angle);
    const PFloat32 s = Sin(angle);
    mat.m11 = c;        mat.m12 = s;        mat.m13 = 0.0f;     mat.m14 = 0.0f;
    mat.m21 = -s;       mat.m22 = c;        mat.m23 = 0.0f;     mat.m24 = 0.0f;
    mat.m31 = 0.0f;     mat.m32 = 0.0f;     mat.m33 = 1.0f;     mat.m34 = 0.0f;
    mat.m41 = 0.0f;     mat.m42 = 0.0f;     mat.m43 = 0.0f;     mat.m44 = 1.0f;
}

//----------------------------------------------------------------------------------------

void SetProjection(
    Mat44InOut& mat,
    const PFloat32& l,
    const PFloat32& r,
    const PFloat32& t,
    const PFloat32& b,
    const PFloat32& n,
    const PFloat32& f
    )
{
    const PFloat32 two_n = 2.0f*n;
    const PFloat32 r_m_l = r - l;
    const PFloat32 t_m_b = t - b;
    const PFloat32 f_m_n = f - n;
    mat.m11 =  (two_n)/(r_m_l); mat.m12 = 0.0f;              mat.m13 = (r+l)/(r_m_l);  mat.m14=0.0f; 
    mat.m21 =  0.0f;            mat.m22 = (two_n)/(t_m_b);   mat.m23 = (t+b)/(t_m_b);  mat.m24=0.0f;
#if PEGASUS_GAPI_DX
    //domain goes from 0 to 1 on Z
    mat.m31 =  0.0f;            mat.m32 = 0.0f;              mat.m33 = -(f)/(f_m_n); mat.m34=-(n*f)/(f_m_n);
#else
    //domain goes from -1 to 1 on Z
    mat.m31 =  0.0f;            mat.m32 = 0.0f;              mat.m33 = -(f+n)/(f_m_n); mat.m34=-(two_n*f)/(f_m_n);
#endif
    mat.m41 =  0.0f;            mat.m42 = 0.0f;              mat.m43 = -1.0f;          mat.m44=0.0f;
}

//----------------------------------------------------------------------------------------

void SetProjection(
    Mat44InOut& mat,
    const PFloat32& fov,
    const PFloat32& aspect, // y / x
    const PFloat32& n,
    const PFloat32& f
    )
{
    const PFloat32 r = n*Tan(0.5f*fov);
    const PFloat32 t = aspect * r;
    SetProjection(mat,-r,r,t,-t,n,f);
}

//----------------------------------------------------------------------------------------

//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(Mat22In mat)
//{
//    String string(mat.m11);
//    string += ",\t";
//    string += String(mat.m12);
//    string += "\n";
//
//    string += String(mat.m21);
//    string += ",\t";
//    string += String(mat.m22);
//    string += "\n";
//
//    DebugOutputs::DebugMessage(string);
//}

//----------------------------------------------------------------------------------------

//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(Mat33In mat)
//{
//    String string(mat.m11);
//    string += ",\t";
//    string += String(mat.m12);
//    string += ",\t";
//    string += String(mat.m13);
//    string += "\n";
//
//    string += String(mat.m21);
//    string += ",\t";
//    string += String(mat.m22);
//    string += ",\t";
//    string += String(mat.m23);
//    string += "\n";
//
//    string += String(mat.m31);
//    string += ",\t";
//    string += String(mat.m32);
//    string += ",\t";
//    string += String(mat.m33);
//    string += "\n";
//
//    DebugOutputs::DebugMessage(string);
//}

//----------------------------------------------------------------------------------------

//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(Mat44In mat)
//{
//    String string(mat.m11);
//    string += ",\t";
//    string += String(mat.m12);
//    string += ",\t";
//    string += String(mat.m13);
//    string += ",\t";
//    string += String(mat.m14);
//    string += "\n";
//
//    string += String(mat.m21);
//    string += ",\t";
//    string += String(mat.m22);
//    string += ",\t";
//    string += String(mat.m23);
//    string += ",\t";
//    string += String(mat.m24);
//    string += "\n";
//
//    string += String(mat.m31);
//    string += ",\t";
//    string += String(mat.m32);
//    string += ",\t";
//    string += String(mat.m33);
//    string += ",\t";
//    string += String(mat.m34);
//    string += "\n";
//
//    string += String(mat.m41);
//    string += ",\t";
//    string += String(mat.m42);
//    string += ",\t";
//    string += String(mat.m43);
//    string += ",\t";
//    string += String(mat.m44);
//    string += "\n";
//
//    DebugOutputs::DebugMessage(string);
//}

//----------------------------------------------------------------------------------------

//! \todo Implement conversion to string for the debug output
//void WriteMatToDebugOutputs(PFloat32 ** mat, PUInt32 nRows, PUInt32 nColumns)
//{
//    PG_ASSERT(mat);
//    PG_ASSERT(nRows >= 1);
//    PG_ASSERT(nColumns >= 1);
//
//    String string;
//    PUInt32 r, c;
//    for (r = 0; r < nRows; r++)
//    {
//        for (c = 0; c < nColumns - 1; c++)
//        {
//            string += String(mat[r][c]);
//            string += ",\t";
//        }
//        string += String(mat[r][nColumns - 1]);
//        string += "\n";
//    }
//
//    DebugOutputs::DebugMessage(string);
//}


}   // namespace Math
}   // namespace Pegasus
