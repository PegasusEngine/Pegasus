/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Time_Win32.cpp
//! \author	Karolyn Boulanger
//! \date	19th December 2013
//! \brief	Time handling, particularly for the timeline (Win32 implementation)

#if PEGASUS_PLATFORM_WINDOWS

#include "Pegasus/Core/Time.h"
#include "Pegasus/Core/Log.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Pegasus {
namespace Core {


//! True if the performance counter is supported (uses the low-precision system counter otherwise)
static bool gPerfCounterSupported = false;

//! Inverse of the performance counter frequency when supported
static double gPerfCounterPrecision = 0.001;

//! Current Pegasus time, in seconds
//! \warning This value is not guaranteed to start at 0.0 when the application starts
static double gCurrentPegasusTime = 0.0;

//----------------------------------------------------------------------------------------

void InitializePegasusTime()
{
    PG_LOG('TIME', "Initializing the time system");

    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency))
    {
        PG_LOG('TIME', "Performance counter supported. Frequency: %I64u Hz", frequency.QuadPart);

        gPerfCounterSupported = true;
        gPerfCounterPrecision = 1.0 / static_cast<double>(frequency.QuadPart);
    }
    else
    {
        PG_LOG('TIME', "Performance counter unsupported. Using the system counter instead");

        gPerfCounterSupported = false;
        gPerfCounterPrecision = 0.0;
    }

    gCurrentPegasusTime = 0.0;
    UpdatePegasusTime();
}

//----------------------------------------------------------------------------------------

void UpdatePegasusTime()
{
    if (gPerfCounterSupported)
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        gCurrentPegasusTime = static_cast<double>(counter.QuadPart) * gPerfCounterPrecision;
    }
    else
    {
        gCurrentPegasusTime = static_cast<double>(GetTickCount()) * 0.001;
    }
}

//----------------------------------------------------------------------------------------

double GetPegasusTime()
{
    return gCurrentPegasusTime;
}


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
