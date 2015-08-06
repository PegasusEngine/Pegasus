/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Types.h
//! \author Karolyn Boulanger
//! \date   1st September 2013
//! \brief  Base types for the math library

#ifndef PEGASUS_MATH_TYPES_H
#define PEGASUS_MATH_TYPES_H

#include "Pegasus/Preprocessor.h"

namespace Pegasus {
namespace Math {


//! 8 bits unsigned integer base type
typedef unsigned char PUInt8;

//! 8 bits signed integer base type
typedef signed char PInt8;

//----------------------------------------------------------------------------------------

//! 16 bits unsigned integer base type
typedef unsigned short PUInt16;

//! 16 bits signed integer base type
typedef signed short PInt16;

//----------------------------------------------------------------------------------------

//! 32 bits unsigned integer base type
typedef unsigned long PUInt32;

//! 32 bits signed integer base type
typedef signed long PInt32;

//----------------------------------------------------------------------------------------

#if PEGASUS_PLATFORM_WINDOWS && !PEGASUS_COMPILER_GCC && !PEGASUS_COMPILER_UNKNOWN

//! 64 bits unsigned integer
typedef unsigned __int64 PUInt64;

//! 64 bits signed integer
typedef __int64 PInt64;

//! 64 bits unsigned integer constant macro (appends the suffix)
#define PCST_UINT64(c) c##ui64

//! 64 bits signed integer constant macro (appends the suffix)
#define PCST_INT64(c) c##i64

#else

//! 64 bits unsigned integer
typedef unsigned long long PUInt64;

//! 64 bits signed integer
typedef long long PInt64;

//! 64 bits unsigned integer constant macro (appends the suffix)
#define PCST_UINT64(c) c##ULL

//! 64 bits signed integer constant macro (appends the suffix)
#define PCST_INT64(c) c##LL

#endif

//----------------------------------------------------------------------------------------

//! 32 bits floating point number
typedef float PFloat32;

//! 32 bits floating point number
typedef float PFloat;

//----------------------------------------------------------------------------------------

//! 64 bits floating point number
typedef double PFloat64;

//! 64 bits floating point number
typedef double PDouble;


}   // namespace Math
}   // namespace Pegasus

#endif  // PEGASUS_MATH_TYPES_H
