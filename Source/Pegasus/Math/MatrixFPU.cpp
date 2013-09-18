/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MatrixFPU.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Matrix operations (with other matrices and vectors) (FPU implementation)

#include "Pegasus/Math/MatrixFPU.h"

namespace Pegasus {
namespace Math {


#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

//#pragma check_stack (off)
//#pragma warning (disable:4035)

/*__declspec(naked) void Mult22_21FPU(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+4 ]
        fmul    dword ptr [edx+4 ]
        fadd

        fld        dword ptr [ecx+8]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+12]
        fmul    dword ptr [edx+4 ]
        fadd

        fstp    dword ptr [eax+4 ]        // Save the results int dst
        fstp    dword ptr [eax   ]

        ret
    }
}

//----------------------------------------------------------------------------------------

__declspec(naked) void Mult33_31FPU(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+4 ]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+8 ]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fld        dword ptr [ecx+12]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+16]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+20]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fld        dword ptr [ecx+24]        // dst[2] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+28]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+32]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fstp    dword ptr [eax+8 ]        // Save the results int dst
        fstp    dword ptr [eax+4 ]
        fstp    dword ptr [eax   ]

        ret
    }
}
*/
//----------------------------------------------------------------------------------------

__declspec(naked) void Mult44_41FPU(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+4 ]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+8 ]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+12]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+16]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+20]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+24]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+28]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+32]        // dst[2] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+36]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+40]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+44]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+48]        // dst[3] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+52]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+56]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+60]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fstp    dword ptr [eax+12]        // Save the results int dst
        fstp    dword ptr [eax+8 ]
        fstp    dword ptr [eax+4 ]
        fstp    dword ptr [eax   ]

        ret
    }
}

//----------------------------------------------------------------------------------------
/*
__declspec(naked) void Mult22T_21FPU(Vec2InOut dst, Mat22In mat, Vec2In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+8 ]
        fmul    dword ptr [edx+4 ]
        fadd

        fld        dword ptr [ecx+4 ]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+12]
        fmul    dword ptr [edx+4 ]
        fadd

        fstp    dword ptr [eax+4 ]        // Save the results in dst
        fstp    dword ptr [eax   ]

        ret
    }
}

//----------------------------------------------------------------------------------------

__declspec(naked) void Mult33T_31FPU(Vec3InOut dst, Mat33In mat, Vec3In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+12]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+24]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fld        dword ptr [ecx+4 ]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+16]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+28]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fld        dword ptr [ecx+8 ]        // dst[2] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+20]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+32]
        fmul    dword ptr [edx+8 ]
        fadd
        fadd

        fstp    dword ptr [eax+8 ]        // Save the results int dst
        fstp    dword ptr [eax+4 ]
        fstp    dword ptr [eax   ]

        ret
    }
}
*/
//----------------------------------------------------------------------------------------

__declspec(naked) void Mult44T_41FPU(Vec4InOut dst, Mat44In mat, Vec4In vec)
{
    __asm
    {
        mov        eax, dword ptr [esp+4 ]    // Load the addresses of the parameters
        mov        ecx, dword ptr [esp+8 ]
        mov        edx, dword ptr [esp+12]

        fld        dword ptr [ecx   ]        // dst[0] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+16]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+32]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+48]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+4 ]        // dst[1] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+20]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+36]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+52]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+8 ]        // dst[2] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+24]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+40]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+56]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fld        dword ptr [ecx+12]        // dst[3] calculation
        fmul    dword ptr [edx   ]
        fld        dword ptr [ecx+28]
        fmul    dword ptr [edx+4 ]
        fld        dword ptr [ecx+44]
        fmul    dword ptr [edx+8 ]
        fld        dword ptr [ecx+60]
        fmul    dword ptr [edx+12]
        fadd
        fadd
        fadd

        fstp    dword ptr [eax+12]        // Save the results int dst
        fstp    dword ptr [eax+8 ]
        fstp    dword ptr [eax+4 ]
        fstp    dword ptr [eax   ]

        ret
    }
}

//#pragma warning (default:4035)
//#pragma check_stack

#endif    // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC

/*
#include <xmmintrin.h>

#define SAMPLES 100

long start = 0;
long end = 0;
long save_ebx;

#define RecordTime(var) \
__asm cpuid \
__asm rdtsc \
__asm mov var, eax


#define StartRecordTime \
__asm mov save_ebx, ebx \
RecordTime(start)


#define StopRecordTime \
RecordTime(end) \
__asm mov ebx, save_ebx


int i = 0;
long base = 0;
long tick = 0;
long ticks[SAMPLES];

int Duration( int sz = SAMPLES)
{
    long nclocks = 0;
    for ( int i = 0 ; i < sz; i++) {
    if (!nclocks || ticks[i] < nclocks)
    nclocks = ticks[i];
    }
    return int(nclocks - base);
}


void report( char* format, ...)
{
    va_list marker;
    char buf[500];
    vsprintf(buf, format, va_start(marker, format));
    puts(buf);
    // OutputDebugString(buf); OutputDebugString("\n");
}


#define START_MEASUREMENTS \
SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL); \
for (i = 0 ; i < SAMPLES; i++) {

#define END_MEASUREMENTS \
ticks[i] = end - start; \
} \
SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL); \
report("Duration for %s:\t%i", testname, Duration());


// Offset for mat[i][j], w is an row width, t == 1 for transposed access.
#define mi(w, t, i, j) 4 * ((i * w + j) * (1-t) + (j * w + i) * t)

// Load & multiply.
#define flm(k, i, j, m, n, a, b) \
__asm fld dword ptr [edx + mi(m, a, i, k)] \
__asm fmul dword ptr [ecx + mi(n, b, k, j)]

// Load, multiply & add.
#define flma(k, i, j, m, n, a, b) flm(k, i, j, m, n, a, b) __asm faddp ST(1), ST(0)

#define e3(i, j, l, m, n, a, b) \
flm (0, i, j, m, n, a, b) \
flma(1, i, j, m, n, a, b) \
flma(2, i, j, m, n, a, b) \
__asm fstp dword ptr [eax + mi(l, 0, i, j)]


void PII_Mult_3x3_3x3( float *src1, float *src2, float *dst)
{
    StartRecordTime;
    __asm mov edx, DWORD PTR src1
    __asm mov ecx, DWORD PTR src2
    __asm mov eax, DWORD PTR dst
    e3(0, 0, 3, 3, 3, 0, 0) e3(0, 1, 3, 3, 3, 0, 0) e3(0, 2, 3, 3, 3, 0, 0)
    e3(1, 0, 3, 3, 3, 0, 0) e3(1, 1, 3, 3, 3, 0, 0) e3(1, 2, 3, 3, 3, 0, 0)
    e3(2, 0, 3, 3, 3, 0, 0) e3(2, 1, 3, 3, 3, 0, 0) e3(2, 2, 3, 3, 3, 0, 0)
    StopRecordTime;
}


#define e4(i, j, l, m, n, a, b) \
flm(0, i, j, m, n, a, b) \
flm(1, i, j, m, n, a, b) \
flm(2, i, j, m, n, a, b) \
flm(3, i, j, m, n, a, b) \
__asm faddp st(1), st(0) \
__asm fxch st(2) \
__asm faddp st(1), st(0) \
__asm faddp st(1), st(0) \
__asm fstp dword ptr [eax + mi(l, 0, i, j)]


void PII_Mult00_4x4_4x4( float *src1, float *src2, float *dst)
{
    StartRecordTime;
    __asm mov edx, DWORD PTR src1
    __asm mov ecx, DWORD PTR src2
    __asm mov eax, DWORD PTR dst
    e4(0, 0, 4, 4, 4, 0, 0)
    e4(0, 1, 4, 4, 4, 0, 0)
    e4(0, 2, 4, 4, 4, 0, 0)
    e4(0, 3, 4, 4, 4, 0, 0)
    e4(1, 0, 4, 4, 4, 0, 0)
    e4(1, 1, 4, 4, 4, 0, 0)
    e4(1, 2, 4, 4, 4, 0, 0)
    e4(1, 3, 4, 4, 4, 0, 0)
    e4(2, 0, 4, 4, 4, 0, 0)
    e4(2, 1, 4, 4, 4, 0, 0)
    e4(2, 2, 4, 4, 4, 0, 0)
    e4(2, 3, 4, 4, 4, 0, 0)
    e4(3, 0, 4, 4, 4, 0, 0)
    e4(3, 1, 4, 4, 4, 0, 0)
    e4(3, 2, 4, 4, 4, 0, 0)
    e4(3, 3, 4, 4, 4, 0, 0)
    StopRecordTime;
}


#define e6(i, j, l, m, n, a, b) \
flm(0, i, j, m, n, a, b) \
flm(1, i, j, m, n, a, b) \
flm(2, i, j, m, n, a, b) \
flm(3, i, j, m, n, a, b) \
flm(4, i, j, m, n, a, b) \
flm(5, i, j, m, n, a, b) \
__asm faddp st(1), st(0) \
__asm fxch st(2) \
__asm faddp st(1), st(0) \
__asm faddp st(1), st(0) \
__asm fxch st(2) \
__asm faddp st(1), st(0) \
__asm faddp st(1), st(0) \
__asm fstp dword ptr [eax + mi(l, 0, i, j)]


void PII_Mult00_3x3_3x1( float *src1, float *src2, float *dst)
{
    StartRecordTime;
    __asm {
    mov edx, dword ptr src1
    mov ecx, dword ptr src2
    mov eax, dword ptr dst
    fld dword ptr [ecx]
    fmul dword ptr [edx+24]
    fld dword ptr [ecx]
    fmul dword ptr [edx+12]
    fld dword ptr [ecx]
    fmul dword ptr [edx]
    fld dword ptr [ecx+4]
    fmul dword ptr [edx+4]
    fld dword ptr [ecx+4]
    fmul dword ptr [edx+16]
    fld dword ptr [ecx+4]
    fmul dword ptr [edx+28]
    fxch ST(2)
    faddp ST(3),ST
    faddp ST(3),ST
    faddp ST(3),ST
    fld dword ptr [ecx+8]
    fmul dword ptr [edx+8]
    fld dword ptr [ecx+8]
    fmul dword ptr [edx+20]
    fld dword ptr [ecx+8]
    fmul dword ptr [edx+32]
    fxch ST(2)
    faddp ST(3),ST
    faddp ST(3),ST
    faddp ST(3),ST
    fstp dword ptr [eax]
    fstp dword ptr [eax+4]
    fstp dword ptr [eax+8]
    }
    StopRecordTime;
}



void PII_Inverse_4x4( float* mat)
{
    float d, di;
    di = mat[0];
    mat[0] = d = 1.0f / di;
    mat[4] *= -d;
    mat[8] *= -d;
    mat[12] *= -d;
    mat[1] *= d;
    mat[2] *= d;
    mat[3] *= d;
    mat[5] += mat[4] * mat[1] * di;
    mat[6] += mat[4] * mat[2] * di;
    mat[7] += mat[4] * mat[3] * di;
    mat[9] += mat[8] * mat[1] * di;
    mat[10] += mat[8] * mat[2] * di;
    mat[11] += mat[8] * mat[3] * di;
    mat[13] += mat[12] * mat[1] * di;
    mat[14] += mat[12] * mat[2] * di;
    mat[15] += mat[12] * mat[3] * di;
    di = mat[5];
    mat[5] = d = 1.0f / di;
    mat[1] *= -d;
    mat[9] *= -d;
    mat[13] *= -d;
    mat[4] *= d;
    mat[6] *= d;
    mat[7] *= d;
    mat[0] += mat[1] * mat[4] * di;
    mat[2] += mat[1] * mat[6] * di;
    mat[3] += mat[1] * mat[7] * di;
    mat[8] += mat[9] * mat[4] * di;
    mat[10] += mat[9] * mat[6] * di;
    mat[11] += mat[9] * mat[7] * di;
    mat[12] += mat[13] * mat[4] * di;
    mat[14] += mat[13] * mat[6] * di;
    mat[15] += mat[13] * mat[7] * di;
    di = mat[10];
    mat[10] = d = 1.0f / di;
    mat[2] *= -d;
    mat[6] *= -d;
    mat[14] *= -d;
    mat[8] *= d;
    mat[9] *= d;
    mat[11] *= d;
    mat[0] += mat[2] * mat[8] * di;
    mat[1] += mat[2] * mat[9] * di;
    mat[3] += mat[2] * mat[11] * di;
    mat[4] += mat[6] * mat[8] * di;
    mat[5] += mat[6] * mat[9] * di;
    mat[7] += mat[6] * mat[11] * di;
    mat[12] += mat[14] * mat[8] * di;
    mat[13] += mat[14] * mat[9] * di;
    mat[15] += mat[14] * mat[11] * di;
    di = mat[15];
    mat[15] = d = 1.0f / di;
    mat[3] *= -d;
    mat[7] *= -d;
    mat[11] *= -d;
    mat[12] *= d;
    mat[13] *= d;
    mat[14] *= d;
    mat[0] += mat[3] * mat[12] * di;
    mat[1] += mat[3] * mat[13] * di;
    mat[2] += mat[3] * mat[14] * di;
    mat[4] += mat[7] * mat[12] * di;
    mat[5] += mat[7] * mat[13] * di;
    mat[6] += mat[7] * mat[14] * di;
    mat[8] += mat[11] * mat[12] * di;
    mat[9] += mat[11] * mat[13] * di;
    mat[10] += mat[11] * mat[14] * di;
}

*/


}   // namespace Math
}   // namespace Pegasus
