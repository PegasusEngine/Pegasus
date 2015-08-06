/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MatrixFPU.h
//! \author Karolyn Boulanger
//! \date   31st August 2013
//! \brief  Matrix operations (with other matrices and vectors) (FPU implementation)

#ifndef PEGASUS_MATH_MATRIXFPU_H
#define PEGASUS_MATH_MATRIXFPU_H

#include "Pegasus/Math/Matrix.h"

namespace Pegasus {
namespace Math {


#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (FPU version)
//! \param mat The 2x2 matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The result 2D vector
/*void Mult22_21FPU(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (FPU version)
//! \param mat The 3x3 matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The result 3D vector
void Mult33_31FPU(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/
//! Multiplication of a matrix and a vector, and the result stored in an other vector
//! (FPU version)
//! \param mat The 4x4 matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The result 4D vector
void Mult44_41FPU(Vec4InOut dst, Mat44In mat, Vec4In vec);

//----------------------------------------------------------------------------------------

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (FPU version)
//! \param mat The 2x2 transposed matrix for the multiplication
//! \param vec The 2D vector for the multiplication
//! \param dst The result 2D vector
/*void Mult22T_21FPU(Vec2InOut dst, Mat22In mat, Vec2In vec);

//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (FPU version)
//! \param mat The 3x3 transposed matrix for the multiplication
//! \param vec The 3D vector for the multiplication
//! \param dst The result 3D vector
void Mult33T_31FPU(Vec3InOut dst, Mat33In mat, Vec3In vec);
*/
//! Multiplication of a transposed matrix and a vector,
//! and the result stored in an other vector (FPU version)
//! \param mat The 4x4 transposed matrix for the multiplication
//! \param vec The 4D vector for the multiplication
//! \param dst The result 4D vector
void Mult44T_41FPU(Vec4InOut dst, Mat44In mat, Vec4In vec);


#endif    // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_MATRIXFPU_H
