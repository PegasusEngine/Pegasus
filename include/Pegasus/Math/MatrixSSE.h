/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MatrixSSE.h
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Matrix operations (with other matrices and vectors) (SSE implementation)

#ifndef PEGASUS_MATH_MATRIXSSE_H
#define PEGASUS_MATH_MATRIXSSE_H

#include "Pegasus/Math/Matrix.h"

namespace Pegasus {
namespace Math {


#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

#ifndef _USE_INTEL_COMPILER
#pragma message ("Use Intel C++ Compiler for the compilation of SSE operations")

#else
#pragma message ("Check that Intel C++ Compiler compiles the full project")


//----------------------------------------------------------------------------------------

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (SSE version)
//! \param mat The 2x2 matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The result 2D vector
/*void Mult22_21SSE(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (SSE version)
//! \param mat The 3x3 matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The result 3D vector
void Mult33_31SSE(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/
//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (SSE version)
//! \param mat The 4x4 matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The result 4D vector
void Mult44_41SSE(Vec4InOut dst, Mat44In mat, Vec4In vec);

//----------------------------------------------------------------------------------------

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (SSE version)
//! \param mat The 2x2 transposed matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The result 2D vector
/*void Mult22T_21SSE(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (SSE version)
//! \param mat The 3x3 transposed matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The result 3D vector
void Mult33T_31SSE(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/
//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (SSE version)
//! \param mat The 4x4 transposed matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The result 4D vector
void Mult44T_41SSE(Vec4InOut dst, Mat44In mat, Vec4In vec);


#endif        // _USE_INTEL_COMPILER
#endif        // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC


}   // namespace Math
}   // namespace Pegasus

#endif        // PEGASUS_MATH_MATRIXSSE_H
