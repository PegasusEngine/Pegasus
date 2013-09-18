/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Math.cpp
//! \author Kevin Boulanger
//! \date   31st August 2013
//! \brief  Math library

#include "Pegasus/Math/Math.h"

// Includes specific to the IA32 architecture
#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC
#    ifdef _USE_INTEL_COMPILER
#        include "Pegasus/Math/MatrixSSE.h"
#    endif
#    include "Pegasus/Math/MatrixFPU.h"
#endif

namespace Pegasus {
namespace Math {


void InitMathLibrary()
{
    // The initialization of the math library is only for the IA32 architecture
#if PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC
    
#ifdef _USE_INTEL_COMPILER
    if (Engine::IsSSESupported())
    {
        // SSE functions
        /*Mult22_21 = Mult22_21SSE;    Mult33_31 = Mult33_31SSE;    */Mult44_41 = Mult44_41SSE;
        /*Mult22T_21 = Mult22T_21SSE;    Mult33T_31 = Mult33T_31SSE;    */Mult44T_41 = Mult44T_41SSE;
    }
    else
    {
#endif
        // FPU functions
        /*Mult22_21 = Mult22_21FPU;    Mult33_31 = Mult33_31FPU;    */Mult44_41 = Mult44_41FPU;
        /*Mult22T_21 = Mult22T_21FPU;    Mult33T_31 = Mult33T_31FPU;    */Mult44T_41 = Mult44T_41FPU;

#ifdef _USE_INTEL_COMPILER
    }
#endif

#endif    // PEGASUS_ARCH_IA32 && !PEGASUS_MATH_FORCE_GENERIC
}


}   // namespace Math
}   // namespace Pegasus
