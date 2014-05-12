/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Scalar.h
//! \author Kevin Boulanger
//! \date   10th September 2013
//! \brief  Scalar operations

#ifndef PEGASUS_MATH_SCALAR_H
#define PEGASUS_MATH_SCALAR_H

#include "Pegasus/Math/Types.h"
#include "Pegasus/Math/Constants.h"


/****************************************************************************************/

//! \todo Handle the Intel Compiler properly
//_USE_INTEL_COMPILER
    
//! \todo To properly define in Preprocessor.h
#define PEGASUS_ARCH_IA32                   1

//! Force the generic implementation of the math library
//! (the slowest but the most compatible)
#define PEGASUS_MATH_FORCE_GENERIC          1

//! Force the full precision of the calculations in the math library (slower)
#define PEGASUS_MATH_FULL_PRECISION         0

/****************************************************************************************/


#if !PEGASUS_ARCH_IA32 || PEGASUS_MATH_FORCE_GENERIC || PEGASUS_MATH_FULL_PRECISION
#include <cmath>
#endif

#include <cstdlib>


namespace Pegasus {
namespace Math {


//@{
//! Test if a number is close to another one (usually better to use than == due to precision issues)
//! \param v1 Input number
//! \param v2 Number to compare to
//! \param epsilon Size of the window around f2 for which f1 is considered as equal
//! \return True if f1 is similar to f2 (with less than epsilon difference)
inline bool IsSimilar(PFloat32 v1, PFloat32 v2, PFloat32 epsilon = 1e-5f)
    {
        PG_ASSERT(epsilon > 0.0f);
        return (v1 >= (v2 - epsilon)) && (v1 <= (v2 + epsilon));
    }
inline bool IsSimilar(PFloat64 v1, PFloat64 v2, PFloat64 epsilon = 1e-10)
    {
        PG_ASSERT(epsilon > 0.0);
        return (v1 >= (v2 - epsilon)) && (v1 <= (v2 + epsilon));
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Square of a value
//! \param v Argument value
//! \return v * v
inline PUInt16  Sq(PUInt16  v) { PG_ASSERT(v <= 255);                      return v * v; }
inline PInt16   Sq(PInt16   v) { PG_ASSERT((v >= -255) && (v <= 255));     return v * v; }
inline PUInt32  Sq(PUInt32  v) { PG_ASSERT(v <= 65535);                    return v * v; }
inline PInt32   Sq(PInt32   v) { PG_ASSERT((v >= -65535) && (v <= 65535)); return v * v; }
inline PFloat32 Sq(PFloat32 v) {                                           return v * v; }
inline PFloat64 Sq(PFloat64 v) {                                           return v * v; }
//@}

//@{
//! Square root of a float (faster in most case than the standard sqrt())
//! \param v Value to take the square root from (>= 0.0)
//! \return The square root of v
#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC && !PEGASUS_MATH_FULL_PRECISION
inline PFloat32 Sqrt(PFloat32 v)
    {
        PG_ASSERT(v >= 0.0f);
        if (v == 0.0f) return 0.0f;
        union { PFloat32 f; PUInt32 i; } x;
        x.f = v;
        x.i = 0x5F3759DF - (x.i >> 1);              // first approximation of 1/sqrt(x)
        PFloat32 v2 = v * 0.5f;
        x.f *= 1.5f - x.f * x.f * v2;               // 2 Newton-Raphson iterations
        return v * x.f * (1.5f - x.f * x.f * v2);
    }
#else
inline PFloat32 Sqrt(PFloat32 v)
    {
        PG_ASSERT(v >= 0.0f);
        return sqrtf(v);
    }
#endif

inline PFloat64 Sqrt(PFloat64 v)
    {
        PG_ASSERT(v >= 0.0f);
        return sqrt(v);
    }
//@}

//@{
//! Reciprocal square root of a float
//! \note Usually faster than 1.0 / Sqrt(val),
//!       a * RcpSqrt(b) is faster than a / Sqrt(b))
//! \param v Value to take the reciprocal square root from (> 0.0)
//! \return The reciprocal square root of v
#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC && !PEGASUS_MATH_FULL_PRECISION
inline PFloat32 RcpSqrt(PFloat32 v)
    {
        PG_ASSERT(v > 0.0f);
        union { PFloat32 f; PUInt32 i; } x;
        x.f = v;
        x.i = 0x5F3759DF - (x.i >> 1);              // first approximation of 1/sqrt(x)
        PFloat32 v2 = v * 0.5f;
        x.f *= 1.5f - x.f * x.f * v2;               // 2 Newton-Raphson iterations
        return x.f * (1.5f - x.f * x.f * v2);
    }
#else
inline PFloat32 RcpSqrt(PFloat32 v)
    {
        PG_ASSERT(v > 0.0f);
        return 1.0f / sqrtf(v);
    }
#endif

inline PFloat64 RcpSqrt(PFloat64 v)
    {
        PG_ASSERT(v > 0.0);
        return 1.0 / sqrt(v);
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Power of 2 of n
//! \param n The exponent (0 <= n < 15/31)
//! \return 2^n
inline PUInt8  Pow2(PUInt8  n) { PG_ASSERT(n <= 7);  return 1 << n; }
inline PUInt16 Pow2(PUInt16 n) { PG_ASSERT(n <= 15); return 1 << n; }
inline PUInt32 Pow2(PUInt32 n) { PG_ASSERT(n <= 31); return 1 << n; }
//@}

//@{
//! Is an integer a power of two ?
//! \param n The integer to test
//! \return true if n is a power of two
inline bool IsPowerOf2(PUInt8  n) { return (n == 0) ? false : (((n - 1) & n) == 0); }
inline bool IsPowerOf2(PUInt16 n) { return (n == 0) ? false : (((n - 1) & n) == 0); }
inline bool IsPowerOf2(PUInt32 n) { return (n == 0) ? false : (((n - 1) & n) == 0); }
//@}

//@{
//! Get the next power of 2
//! \param n Argument integer
//! \return Power of 2 integer which is greater than or equal to n
inline PUInt8 NextPowerOf2(PUInt8  n)
    {
        PG_ASSERT(n <= (1 << 7));
        PUInt8 res = 1;
        while (res < n) { res <<= 1; }
        return res;
    }

inline PUInt16 NextPowerOf2(PUInt16 n)
    {
        PG_ASSERT(n <= (1 << 15));
        PUInt16 res = 1;
        while (res < n) { res <<= 1; }
        return res;
    }

inline PUInt32 NextPowerOf2(PUInt32 n)
    {
        PG_ASSERT(n <= (1 << 31));
        PUInt32 res = 1;
        while (res < n) { res <<= 1; }
        return res;
    }
//@}

//@{
//! Get the integer base 2 logarithm
//! \param n Argument integer (>= 1)
//! \return Base 2 logarithm of n, rounded to the smaller integer
inline PUInt8 Log2(PUInt8  n)
    {
        PG_ASSERT(n >= 1);
        if (n == 0) { return 0; }
        PUInt8 res = 0;
        n >>= 1;
        while (n > 0) { res++; n >>= 1; }
        return res;
    }

inline PUInt16 Log2(PUInt16 n)
    {
        PG_ASSERT(n >= 1);
        if (n == 0) { return 0; }
        PUInt8 res = 0;
        n >>= 1;
        while (n > 0) { res++; n >>= 1; }
        return res;
    }

inline PUInt32 Log2(PUInt32 n)
    {
        PG_ASSERT(n >= 1);
        if (n == 0) { return 0; }
        PUInt8 res = 0;
        n >>= 1;
        while (n > 0) { res++; n >>= 1; }
        return res;
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Test if an integer is an even number
//! \param n Integer to test
//! \return true if the integer is an even number
inline bool Even(PUInt8  n) { return (n & 1) == 0; }
inline bool Even(PInt8   n) { return (n & 1) == 0; }
inline bool Even(PUInt16 n) { return (n & 1) == 0; }
inline bool Even(PInt16  n) { return (n & 1) == 0; }
inline bool Even(PUInt32 n) { return (n & 1) == 0; }
inline bool Even(PInt32  n) { return (n & 1) == 0; }
//@}

//@{
//! Test if an integer is an odd number
//! \param n Integer to test
//! \return true if the integer is an odd number
inline bool Odd(PUInt8  n) { return (n & 1) != 0; }
inline bool Odd(PInt8   n) { return (n & 1) != 0; }
inline bool Odd(PUInt16 n) { return (n & 1) != 0; }
inline bool Odd(PInt16  n) { return (n & 1) != 0; }
inline bool Odd(PUInt32 n) { return (n & 1) != 0; }
inline bool Odd(PInt32  n) { return (n & 1) != 0; }
//@}

//@{
//! Test if an integer is a multiple of another integer
//! \param n Integer to test
//! \param m Multiplier (> 0)
//! \return true if n = m * q with q integer and > 0
inline bool IsMultiple(PUInt8  n, PUInt8  m) { PG_ASSERT(m > 0); return (n % m) == 0; }
inline bool IsMultiple(PUInt16 n, PUInt16 m) { PG_ASSERT(m > 0); return (n % m) == 0; }
inline bool IsMultiple(PUInt32 n, PUInt32 m) { PG_ASSERT(m > 0); return (n % m) == 0; }
//@}

//----------------------------------------------------------------------------------------

//! Minimum of 2 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \return The minimum between the two input values
template <typename T>
inline const T & Min(const T & v1, const T & v2) { return v1.operator<(v2) ? v1 : v2; }

//@{
//! Minimum of 2 base types (by value)
//! \note Faster than Min<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \return The minimum between the two input values
inline PUInt8   Min(PUInt8   v1, PUInt8   v2) { return v1 < v2 ? v1 : v2; }
inline PInt8    Min(PInt8    v1, PInt8    v2) { return v1 < v2 ? v1 : v2; }
inline PUInt16  Min(PUInt16  v1, PUInt16  v2) { return v1 < v2 ? v1 : v2; }
inline PInt16   Min(PInt16   v1, PInt16   v2) { return v1 < v2 ? v1 : v2; }
inline PUInt32  Min(PUInt32  v1, PUInt32  v2) { return v1 < v2 ? v1 : v2; }
inline PInt32   Min(PInt32   v1, PInt32   v2) { return v1 < v2 ? v1 : v2; }
inline PFloat32 Min(PFloat32 v1, PFloat32 v2) { return v1 < v2 ? v1 : v2; }
inline PFloat64 Min(PFloat64 v1, PFloat64 v2) { return v1 < v2 ? v1 : v2; }
//@}

//! Maximum of 2 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \return The maximum between the two input values
template <typename T>
inline const T & Max(const T & v1, const T & v2) { return v2.operator<(v1) ? v1 : v2; }

//@{
//! Maximum of 2 base types (by value)
//! \note Faster than Max<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \return The maximum between the two input values
inline PUInt8   Max(PUInt8   v1, PUInt8   v2) { return v1 > v2 ? v1 : v2; }
inline PInt8    Max(PInt8    v1, PInt8    v2) { return v1 > v2 ? v1 : v2; }
inline PUInt16  Max(PUInt16  v1, PUInt16  v2) { return v1 > v2 ? v1 : v2; }
inline PInt16   Max(PInt16   v1, PInt16   v2) { return v1 > v2 ? v1 : v2; }
inline PUInt32  Max(PUInt32  v1, PUInt32  v2) { return v1 > v2 ? v1 : v2; }
inline PInt32   Max(PInt32   v1, PInt32   v2) { return v1 > v2 ? v1 : v2; }
inline PFloat32 Max(PFloat32 v1, PFloat32 v2) { return v1 > v2 ? v1 : v2; }
inline PFloat64 Max(PFloat64 v1, PFloat64 v2) { return v1 > v2 ? v1 : v2; }
//@}

//----------------------------------------------------------------------------------------

//! Minimum of 3 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \return The minimum between the three input values
template <typename T>
inline const T & Min(const T & v1, const T & v2, const T & v3) { return Min(v1, Min(v2, v3)); }

//@{
//! Minimum of 3 base types (by value)
//! \note Faster than Min<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \return The minimum between the three input values
inline PUInt8   Min(PUInt8   v1, PUInt8   v2, PUInt8   v3) { return Min(v1, Min(v2, v3)); }
inline PInt8    Min(PInt8    v1, PInt8    v2, PInt8    v3) { return Min(v1, Min(v2, v3)); }
inline PUInt16  Min(PUInt16  v1, PUInt16  v2, PUInt16  v3) { return Min(v1, Min(v2, v3)); }
inline PInt16   Min(PInt16   v1, PInt16   v2, PInt16   v3) { return Min(v1, Min(v2, v3)); }
inline PUInt32  Min(PUInt32  v1, PUInt32  v2, PUInt32  v3) { return Min(v1, Min(v2, v3)); }
inline PInt32   Min(PInt32   v1, PInt32   v2, PInt32   v3) { return Min(v1, Min(v2, v3)); }
inline PFloat32 Min(PFloat32 v1, PFloat32 v2, PFloat32 v3) { return Min(v1, Min(v2, v3)); }
inline PFloat64 Min(PFloat64 v1, PFloat64 v2, PFloat64 v3) { return Min(v1, Min(v2, v3)); }
//@}

//! Maximum of 3 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \return The maximum between the three input values
template <typename T>
inline const T & Max(const T & v1, const T & v2, const T & v3) { return Max(v1, Max(v2, v3)); }

//@{
//! Maximum of 3 base types (by value)
//! \note Faster than Max<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \return The maximum between the three input values
inline PUInt8   Max(PUInt8   v1, PUInt8   v2, PUInt8   v3) { return Max(v1, Max(v2, v3)); }
inline PInt8    Max(PInt8    v1, PInt8    v2, PInt8    v3) { return Max(v1, Max(v2, v3)); }
inline PUInt16  Max(PUInt16  v1, PUInt16  v2, PUInt16  v3) { return Max(v1, Max(v2, v3)); }
inline PInt16   Max(PInt16   v1, PInt16   v2, PInt16   v3) { return Max(v1, Max(v2, v3)); }
inline PUInt32  Max(PUInt32  v1, PUInt32  v2, PUInt32  v3) { return Max(v1, Max(v2, v3)); }
inline PInt32   Max(PInt32   v1, PInt32   v2, PInt32   v3) { return Max(v1, Max(v2, v3)); }
inline PFloat32 Max(PFloat32 v1, PFloat32 v2, PFloat32 v3) { return Max(v1, Max(v2, v3)); }
inline PFloat64 Max(PFloat64 v1, PFloat64 v2, PFloat64 v3) { return Max(v1, Max(v2, v3)); }
//@}

//----------------------------------------------------------------------------------------

//! Minimum of 4 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \param v4 The fourth value to compare
//! \return The minimum between the four input values
template <typename T>
inline const T & Min(const T & v1, const T & v2, const T & v3, const T & v4) { return Min(v1, Min(v2, Min(v3, v4))); }

//@{
//! Minimum of 4 base types (by value)
//! \note Faster than Min<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \param v4 The fourth value to compare
//! \return The minimum between the four input values
inline PUInt8   Min(PUInt8   v1, PUInt8   v2, PUInt8   v3, PUInt8   v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PInt8    Min(PInt8    v1, PInt8    v2, PInt8    v3, PInt8    v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PUInt16  Min(PUInt16  v1, PUInt16  v2, PUInt16  v3, PUInt16  v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PInt16   Min(PInt16   v1, PInt16   v2, PInt16   v3, PInt16   v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PUInt32  Min(PUInt32  v1, PUInt32  v2, PUInt32  v3, PUInt32  v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PInt32   Min(PInt32   v1, PInt32   v2, PInt32   v3, PInt32   v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PFloat32 Min(PFloat32 v1, PFloat32 v2, PFloat32 v3, PFloat32 v4) { return Min(v1, Min(v2, Min(v3, v4))); }
inline PFloat64 Min(PFloat64 v1, PFloat64 v2, PFloat64 v3, PFloat64 v4) { return Min(v1, Min(v2, Min(v3, v4))); }
//@}

//! Maximum of 4 values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \param v4 The fourth value to compare
//! \return The maximum between the four input values
template <typename T>
inline const T & Max(const T & v1, const T & v2, const T & v3, const T & v4) { return Max(v1, Max(v2, Max(v3, v4))); }

//@{
//! Maximum of 4 base types (by value)
//! \note Faster than Max<T>() for the base types
//! \param v1 The first value to compare
//! \param v2 The second value to compare
//! \param v3 The third value to compare
//! \param v4 The fourth value to compare
//! \return The maximum between the four input values
inline PUInt8   Max(PUInt8   v1, PUInt8   v2, PUInt8   v3, PUInt8   v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PInt8    Max(PInt8    v1, PInt8    v2, PInt8    v3, PInt8    v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PUInt16  Max(PUInt16  v1, PUInt16  v2, PUInt16  v3, PUInt16  v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PInt16   Max(PInt16   v1, PInt16   v2, PInt16   v3, PInt16   v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PUInt32  Max(PUInt32  v1, PUInt32  v2, PUInt32  v3, PUInt32  v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PInt32   Max(PInt32   v1, PInt32   v2, PInt32   v3, PInt32   v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PFloat32 Max(PFloat32 v1, PFloat32 v2, PFloat32 v3, PFloat32 v4) { return Max(v1, Max(v2, Max(v3, v4))); }
inline PFloat64 Max(PFloat64 v1, PFloat64 v2, PFloat64 v3, PFloat64 v4) { return Max(v1, Max(v2, Max(v3, v4))); }
//@}

//----------------------------------------------------------------------------------------

//! Minimum of N values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v The values to compare
//! \param n Number of values to compare (>= 1)
//! \return The minimum between the input values
template <typename T>
inline const T & Min(const T * v, PUInt32 n)
    {
        PG_ASSERT(v);
        PG_ASSERT(n >= 1);
        const T & outMin(v[0]);
        for (PUInt32 i = 1; i < n; i++) { outMin = Min(outMin, v[i]); }
        return outMin;
    }

//! Maximum of N values (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v The values to compare
//! \param n Number of values to compare (>= 1)
//! \return The maximum between the input values
template <typename T>
inline const T & Max(const T * v, PUInt32 n)
    {
        PG_ASSERT(v);
        PG_ASSERT(n >= 1);
        const T & outMax(v[0]);
        for (PUInt32 i = 1; i < n; i++) { outMax = Max(outMax, v[i]); }
        return outMax;
    }

//----------------------------------------------------------------------------------------

//! Clamping of a value (by reference)
//! \warning operator<() has to be defined for the type T
//! \param v Value to clamp
//! \param min Lower bound for the value
//! \param max Upper bound for the value
//! \return Clamped value, min <= Clamp(v, min, max) <= max
template <typename T>
inline const T & Clamp(const T & v, const T & min, const T & max) { return max.operator<(v) ? max : (v.operator<(min) ? min : v); }

//@{
//! Clamping of a value
//! \note Faster than Clamp<T>() for the base types
//! \param v Value to clamp
//! \param min Lower bound for the value
//! \param max Upper bound for the value
//! \return Clamped value, min <= Clamp(v, min, max) <= max
inline PUInt8   Clamp(PUInt8   v, PUInt8   min, PUInt8   max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PInt8    Clamp(PInt8    v, PInt8    min, PInt8    max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PUInt16  Clamp(PUInt16  v, PUInt16  min, PUInt16  max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PInt16   Clamp(PInt16   v, PInt16   min, PInt16   max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PUInt32  Clamp(PUInt32  v, PUInt32  min, PUInt32  max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PInt32   Clamp(PInt32   v, PInt32   min, PInt32   max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PFloat32 Clamp(PFloat32 v, PFloat32 min, PFloat32 max) { return (v > max) ? max : ((v < min) ? min : v); }
inline PFloat64 Clamp(PFloat64 v, PFloat64 min, PFloat64 max) { return (v > max) ? max : ((v < min) ? min : v); }
//@}

//@{
//! Clamping of a floating point value between 0 and 1
//! \param v Value to clamp
//! \return Clamped value, in [0, 1]
inline PFloat32 Saturate(PFloat32 v) { return Clamp(v, 0.0f, 1.0f); }
inline PFloat64 Saturate(PFloat64 v) { return Clamp(v, 0.0, 1.0); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Get the lower rounded value of an input value
//! \param v Value to round
//! \return Lower rounded value of the input value
inline PFloat32 Floor(PFloat32 v) { return floorf(v); }
inline PFloat64 Floor(PFloat64 v) { return floor (v); }
//@}

//@{
//! Get the greater rounded value of an input value
//! \param v Value to round
//! \return Greater rounded value of the input value
inline PFloat32 Ceil(PFloat32 v) { return ceilf(v); }
inline PFloat64 Ceil(PFloat64 v) { return ceil (v); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Sign of a value
//! \param v Value to test
//! \return 1 if v > 0, 0 if v == 0, -1 if v < 0
inline PInt8    Sign(PInt8    v) { return (v == 0   ) ? 0    : ((v > 0   ) ? 1    : -1   ); }
inline PInt16   Sign(PInt16   v) { return (v == 0   ) ? 0    : ((v > 0   ) ? 1    : -1   ); }
inline PInt32   Sign(PInt32   v) { return (v == 0   ) ? 0    : ((v > 0   ) ? 1    : -1   ); }
inline PFloat32 Sign(PFloat32 v) { return (v == 0.0f) ? 0.0f : ((v > 0.0f) ? 1.0f : -1.0f); }
inline PFloat64 Sign(PFloat64 v) { return (v == 0.0 ) ? 0.0  : ((v > 0.0 ) ? 1.0  : -1.0 ); }
//@}

//@{
//! Test if two values have the same sign (0 is considered as positive)
//! \param v1 First input value
//! \param v2 Second input value
//! \return true if both v1 < 0 and v2 < 0,
//!         or if both v1 >= 0 and v2 >= 0
inline bool AreSignsSame(PInt8    v1, PInt8    v2) { return ((v1 >= 0   ) && (v2 >= 0   )) || ((v1 < 0   ) && (v2 < 0   )); }
inline bool AreSignsSame(PInt16   v1, PInt16   v2) { return ((v1 >= 0   ) && (v2 >= 0   )) || ((v1 < 0   ) && (v2 < 0   )); }
inline bool AreSignsSame(PInt32   v1, PInt32   v2) { return ((v1 >= 0   ) && (v2 >= 0   )) || ((v1 < 0   ) && (v2 < 0   )); }
inline bool AreSignsSame(PFloat32 v1, PFloat32 v2) { return ((v1 >= 0.0f) && (v2 >= 0.0f)) || ((v1 < 0.0f) && (v2 < 0.0f)); }
inline bool AreSignsSame(PFloat64 v1, PFloat64 v2) { return ((v1 >= 0.0 ) && (v2 >= 0.0 )) || ((v1 < 0.0 ) && (v2 < 0.0 )); }
//@}

//@{
//! Test if two values have different signs (0 is considered as positive)
//! \param v1 First input value
//! \param v2 Second input value
//! \return true if v1 < 0 and v2 >= 0,
//!         or if v1 >= 0 and v2 < 0
inline bool AreSignsDifferent(PInt8    v1, PInt8    v2) { return ((v1 >= 0   ) && (v2 < 0   )) || ((v1 < 0   ) && (v2 >= 0   )); }
inline bool AreSignsDifferent(PInt16   v1, PInt16   v2) { return ((v1 >= 0   ) && (v2 < 0   )) || ((v1 < 0   ) && (v2 >= 0   )); }
inline bool AreSignsDifferent(PInt32   v1, PInt32   v2) { return ((v1 >= 0   ) && (v2 < 0   )) || ((v1 < 0   ) && (v2 >= 0   )); }
inline bool AreSignsDifferent(PFloat32 v1, PFloat32 v2) { return ((v1 >= 0.0f) && (v2 < 0.0f)) || ((v1 < 0.0f) && (v2 >= 0.0f)); }
inline bool AreSignsDifferent(PFloat64 v1, PFloat64 v2) { return ((v1 >= 0.0 ) && (v2 < 0.0 )) || ((v1 < 0.0 ) && (v2 >= 0.0 )); }
//@}

//@{
//! Absolute value
//! \param v Input value
//! \return Absolute value of the input value
inline PInt8    Abs(PInt8    v) { return (v < 0    ? -v : v); }
inline PInt16   Abs(PInt16   v) { return (v < 0    ? -v : v); }
inline PInt32   Abs(PInt32   v) { return (v < 0    ? -v : v); }
inline PFloat32 Abs(PFloat32 v) { return (v < 0.0f ? -v : v); }
inline PFloat64 Abs(PFloat64 v) { return (v < 0.0  ? -v : v); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Remainder of x/y
//! \param x Numerator
//! \param y Denominator
//! \return Remainder of x/y (same sign as x)
inline PUInt8   Mod(PUInt8   x, PUInt8   y) { return x % y; }
inline PInt8    Mod(PInt8    x, PInt8    y) { return x % y; }
inline PUInt16  Mod(PUInt16  x, PUInt16  y) { return x % y; }
inline PInt16   Mod(PInt16   x, PInt16   y) { return x % y; }
inline PUInt32  Mod(PUInt32  x, PUInt32  y) { return x % y; }
inline PInt32   Mod(PInt32   x, PInt32   y) { return x % y; }
inline PFloat32 Mod(PFloat32 x, PFloat32 y) { return fmodf(x, y); }
inline PFloat64 Mod(PFloat64 x, PFloat64 y) { return fmod (x, y); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Linear interpolation of two values
//! \param v1 Start value (for t = 0)
//! \param v2 End value (for t = 1)
//! \param t Interpolation coefficient (between 0 and 1 to be valid,
//!          extrapolation occurs if outside these bounds)
//! \return Linear interpolation of v1 and v2 (= (1-t)*v1 + t*v2)
inline PFloat32 Lerp(PFloat32 v1, PFloat32 v2, PFloat32 t) { return v1 + t * (v2 - v1); }
inline PFloat64 Lerp(PFloat64 v1, PFloat64 v2, PFloat64 t) { return v1 + t * (v2 - v1); }
//@}

//@{
//! Cubic interpolation between two edges
//! \param edge0 Lower bound of the interpolation region
//! \param edge1 Upper bound of the interpolation region
//! \param t Input value for the interpolator
//! \return 0 if t <= edge0, 1 if t >= edge1, and cubic interpolation
//!         between edge0 and edge1
inline PFloat32 SmoothStep(PFloat32 edge0, PFloat32 edge1, PFloat32 t)
    {
        t = Clamp((t - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

inline PFloat64 SmoothStep(PFloat64 edge0, PFloat64 edge1, PFloat64 t)
    {
        t = Clamp((t - edge0) / (edge1 - edge0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
    }
//@}

//----------------------------------------------------------------------------------------

//! Reset the pseudo-random number generator.
//! Uses the chrono to randomly initialize the generator
void SRand();

//! Reset the pseudo-random number generator
//! \param seed Seed value. All Rand() calls will be the same for a given seed
void SRand(PUInt32 seed);

//! Get a random number in [0.0f, 1.0f[
//! \return Random number in [0.0f, 1.0[ (granularity is about 0.00003f)
inline PFloat32 Rand() { return PFloat32(rand()) / PFloat32(RAND_MAX + 1); }

//@{
//! Get a random number in { 0, ..., max-1 }
//! \warning Avoid high numbers for n
//!          (> 10000 results in really non-uniform distributions)
//! \param max Excluded maximum value (<= 32768)
//! \return Random number in { 0, ..., max-1 } (granularity is about n / 32768)
inline PUInt16 Rand(PUInt16 max)
    {
        PG_ASSERT(max <= PUInt16(RAND_MAX + 1));
        return PUInt16(rand()) % max;
    }

inline PUInt32 Rand(PUInt32 max)
    {
        PG_ASSERT(max <= PUInt32(RAND_MAX + 1));
        return PUInt32(rand()) % max;
    }
//@}

//@{
//! Get a random number in [0.0, max[
//! \param max Excluded maximum value (> 0.0)
//! \return Random number in [0.0, max[ (granularity is about max * 0.00003)
inline PFloat32 Rand(PFloat32 max)
    {
        PG_ASSERT(max > 0.0f);
        return max * PFloat32(rand()) / PFloat32(RAND_MAX + 1);
    }

inline PFloat64 Rand(PFloat64 max)
    {
        PG_ASSERT(max > 0.0);
        return max * PFloat64(rand()) / PFloat64(RAND_MAX + 1);
    }
//@}

//@{
//! Get a random number in { min, ..., max-1 }
//! \warning Avoid large intervals between min and max
//!          ((max-min) > 10000 results in really non-uniform distributions)
//! \param min Included minimum value
//! \param max Excluded maximum value
//! \return Random number in { min, ..., max-1 } (granularity is about (max-min) / 32768)
inline PUInt16 Rand(PUInt16 min, PUInt16 max)
    {
        PG_ASSERT(min < max);
        return min + PUInt16(rand()) % (max - min);
    }

inline PUInt32 Rand(PUInt32 min, PUInt32 max)
    {
        PG_ASSERT(min < max);
        return min + PUInt32(rand()) % (max - min);
    }
//@}

//@{
//! Get a random number in [min, max[
//! \param min Included minimum value
//! \param max Excluded maximum value
//! \return Random number in [min, max[ (granularity is about (max-min) * 0.00003)
inline PFloat32 Rand(PFloat32 min, PFloat32 max)
    {
        PG_ASSERT(min <= max);
        return min + (max - min) * PFloat32(rand()) / PFloat32(RAND_MAX + 1);
    }

inline PFloat64 Rand(PFloat64 min, PFloat64 max)
    {
        PG_ASSERT(min <= max);
        return min + (max - min) * PFloat64(rand()) / PFloat64(RAND_MAX + 1);
    }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Conversion from radians to degrees
//! \param angle Angle to convert (in radians)
//! \return Converted angle (in degrees)
inline PFloat32 RadToDeg(PFloat32 angle) { return angle * (180.0f / P_PI ); }
inline PFloat64 RadToDeg(PFloat64 angle) { return angle * (180.0  / P_PId); }
//@}

//@{
//! Conversion from degrees to radians
//! \param angle Angle to convert (in degrees)
//! \return Converted angle (in radians)
inline PFloat32 DegToRad(PFloat32 angle) { return angle * (P_PI  / 180.0f); }
inline PFloat64 DegToRad(PFloat64 angle) { return angle * (P_PId / 180.0 ); }
//@}

//----------------------------------------------------------------------------------------

// Trigonometric functions for float numbers

//@{
//! Cosine of an angle
//! \param angle Angle in radians
//! \return Cosine of the input angle (in [-1,1])
inline PFloat32 Cos(PFloat32 angle) { return cosf(angle); }
inline PFloat64 Cos(PFloat64 angle) { return cos (angle); }
//@}

//@{
//! Sine of an angle
//! \param angle Angle in radians
//! \return Sine of the input angle (in [-1,1])
inline PFloat32 Sin(PFloat32 angle) { return sinf(angle); }
inline PFloat64 Sin(PFloat64 angle) { return sin (angle); }
//@}

//@{
//! Tangent of an angle
//! \param angle Angle in radians
//! \return Tangent of the input angle
inline PFloat32 Tan(PFloat32 angle) { return tanf(angle); }
inline PFloat64 Tan(PFloat64 angle) { return tan (angle); }
//@}

//@{
//! Cotangent of an angle (= 1 / Tan(angle))
//! \param angle Angle in radians (for which Tan(angle) != 0)
//! \return Cotangent of the input angle
inline PFloat32 Cotan(PFloat32 angle)
    {
        const PFloat32 t = Tan(angle);
        PG_ASSERT(t != 0.0f);
        return (t == 0.0f) ? 0.0f : (1.0f / t);
    }

inline PFloat64 Cotan(PFloat64 angle)
    {
        const PFloat64 t = Tan(angle);
        PG_ASSERT(t != 0.0);
        return (t == 0.0) ? 0.0 : (1.0 / t);
    }
//@}

//@{
//! Arccosine of a value
//! \param x Input value (in [-1,1])
//! \return Arccosine of the input value (in [0,pi])
inline PFloat32 Acos(PFloat32 x) { PG_ASSERT((x >= -1.0f) && (x <= 1.0f)); return acosf(x); }
inline PFloat64 Acos(PFloat64 x) { PG_ASSERT((x >= -1.0 ) && (x <= 1.0 )); return acos (x); }
//@}

//@{
//! Arcsine of a value
//! \param x Input value (in [-1,1])
//! \return Arcsine of the input value (in [-pi/2,pi/2])
inline PFloat32 Asin(PFloat32 x) { PG_ASSERT((x >= -1.0f) && (x <= 1.0f)); return asinf(x); }
inline PFloat64 Asin(PFloat64 x) { PG_ASSERT((x >= -1.0 ) && (x <= 1.0 )); return asin (x); }
//@}

//@{
//! Arctangent of a value
//! \param x Input value
//! \return Arctangent of the input value (in ]-pi/2,pi/2[)
inline PFloat32 Atan(PFloat32 x) { return atanf(x); }
inline PFloat64 Atan(PFloat64 x) { return atan (x); }
//@}

//@{
//! Arctangent of the division of two values
//! \param y Numerator
//! \param x Denominator (can be equal to 0)
//! \return Arctangent of y/x (in ]-pi/2,pi/2[, 0 if x = 0)
inline PFloat32 Atan2(PFloat32 y, PFloat32 x) { return atan2f(y, x); }
inline PFloat64 Atan2(PFloat64 y, PFloat64 x) { return atan2 (y, x); }
//@}

//----------------------------------------------------------------------------------------

//@{
//! Get x raised to the power of y
//! \param x Base
//! \param y Exponent (>= 0 if x = 0)
//! \return x^y
inline PFloat32 Pow(PFloat32 x, PFloat32 y) { if (x == 0.0f) { PG_ASSERT(y >= 0.0f); } return powf(x, y); }
inline PFloat64 Pow(PFloat64 x, PFloat64 y) { if (x == 0.0 ) { PG_ASSERT(y >= 0.0 ); } return pow (x, y); }
//@}

//@{
//! Natural logarithm of a value
//! \param x Input value (> 0.0)
//! \return Natural logarithm of x
inline PFloat32 Log(PFloat32 x) { PG_ASSERT(x > 0.0f); return logf(x); }
inline PFloat64 Log(PFloat64 x) { PG_ASSERT(x > 0.0 ); return log (x); }
inline PFloat32 Ln (PFloat32 x) { PG_ASSERT(x > 0.0f); return logf(x); }
inline PFloat64 Ln (PFloat64 x) { PG_ASSERT(x > 0.0 ); return log (x); }
//@}

//@{
//! Base 10 logarithm of a value
//! \param x Input value (> 0.0)
//! \return Base 10 logarithm of x
inline PFloat32 Log10(PFloat32 x) { PG_ASSERT(x > 0.0f); return log10f(x); }
inline PFloat64 Log10(PFloat64 x) { PG_ASSERT(x > 0.0 ); return log10 (x); }
//@}

//@{
//! Exponential of a value
//! \param x Input value
//! \return Exponential of x (> 0)
inline PFloat32 Exp(PFloat32 x) { return expf(x); }
inline PFloat64 Exp(PFloat64 x) { return exp (x); }
//@}


}   // namespace Math
}   // namespace Pegasus

#endif    // PEGASUS_MATH_SCALAR_H
