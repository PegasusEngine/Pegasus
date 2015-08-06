/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Constants.h
//! \author Karolyn Boulanger
//! \date   10th September 2013
//! \brief  Math constants

#ifndef PEGASUS_MATH_CONSTANTS_H
#define PEGASUS_MATH_CONSTANTS_H

#include "Pegasus/Math/Types.h"

#include <cfloat>


namespace Pegasus {
namespace Math {


// Float number limits

//! Smallest number such that 1.0f + N3D_FLOAT_EPSILON != 1.0f (about 1e-7)
#define PFLOAT_EPSILON        FLT_EPSILON

//! Minimum positive value (about 1e-38)
#define PFLOAT_MIN            FLT_MIN  

//! Maximum positive value (about 3e38)
#define PFLOAT_MAX            FLT_MAX

//----------------------------------------------------------------------------------------

//@{
//! PI related constants (float version)
const PFloat32 P_4_PI           = 12.566370614359172953850573533118f;
const PFloat32 P_2_PI           = 6.283185307179586476925286766559f;
const PFloat32 P_PI             = 3.1415926535897932384626433832795f;
const PFloat32 P_PI_OVER_2      = 1.5707963267948966192313216916398f;
const PFloat32 P_PI_OVER_3      = 1.0471975511965977461542144610932f;
const PFloat32 P_PI_OVER_4      = 0.7853981633974483096156608458199f;
const PFloat32 P_PI_OVER_6      = 0.5235987755982988730771072305466f;
const PFloat32 P_2_OVER_PI      = 0.63661977236758134307553505349006f;
const PFloat32 P_1_OVER_PI      = 0.31830988618379067153776752674503f;
const PFloat32 P_1_OVER_2_PI    = 0.15915494309189533576888376337251f;
const PFloat32 P_SQ_PI          = 9.8696044010893586188344909998762f;
//@}

//@{
//! PI related constants (double version)
const PFloat64 P_4_PId          = 12.566370614359172953850573533118;
const PFloat64 P_2_PId          = 6.283185307179586476925286766559;
const PFloat64 P_PId            = 3.1415926535897932384626433832795;
const PFloat64 P_PI_OVER_2d     = 1.5707963267948966192313216916398;
const PFloat64 P_PI_OVER_3d     = 1.0471975511965977461542144610932;
const PFloat64 P_PI_OVER_4d     = 0.7853981633974483096156608458199;
const PFloat64 P_PI_OVER_6d     = 0.5235987755982988730771072305466;
const PFloat64 P_2_OVER_PId     = 0.63661977236758134307553505349006;
const PFloat64 P_1_OVER_PId     = 0.31830988618379067153776752674503;
const PFloat64 P_1_OVER_2_PId   = 0.15915494309189533576888376337251;
const PFloat64 P_SQ_PId         = 9.8696044010893586188344909998762;
//@}

//@{
//! Square root related constants (float version)
const PFloat32 P_SQRT_2         = 1.4142135623730950488016887242097f;
const PFloat32 P_SQRT_3         = 1.7320508075688772935274463415059f;
const PFloat32 P_SQRT_5         = 2.2360679774997896964091736687313f;
const PFloat32 P_SQRT_6         = 2.4494897427831780981972840747059f;
const PFloat32 P_1_OVER_SQRT_2  = 0.70710678118654752440084436210485f;
const PFloat32 P_1_OVER_SQRT_3  = 0.57735026918962576450914878050196f;
const PFloat32 P_1_OVER_SQRT_5  = 0.44721359549995793928183473374626f;
const PFloat32 P_1_OVER_SQRT_6  = 0.40824829046386301636621401245098f;
//@}

//@{
//! Square root related constants (double version)
const PFloat64 P_SQRT_2d        = 1.4142135623730950488016887242097;
const PFloat64 P_SQRT_3d        = 1.7320508075688772935274463415059;
const PFloat64 P_SQRT_5d        = 2.2360679774997896964091736687313;
const PFloat64 P_SQRT_6d        = 2.4494897427831780981972840747059;
const PFloat64 P_1_OVER_SQRT_2d = 0.70710678118654752440084436210485;
const PFloat64 P_1_OVER_SQRT_3d = 0.57735026918962576450914878050196;
const PFloat64 P_1_OVER_SQRT_5d = 0.44721359549995793928183473374626;
const PFloat64 P_1_OVER_SQRT_6d = 0.40824829046386301636621401245098;
//@}


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_CONSTANTS_H
