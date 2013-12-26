/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Time.h
//! \author	Kevin Boulanger
//! \date	19th December 2013
//! \brief	Time handling, particularly for the timeline

#ifndef PEGASUS_CORE_TIME_H
#define PEGASUS_CORE_TIME_H

namespace Pegasus {
namespace Core {


//! Initialize the time counter
//! \warning Has to be done before any call to \a GetCurrentTime()
void InitializePegasusTime();

//! Update the current system time
//! \note This is where the system time is actually read, not in GetCurrentTime().
//!       This is an optimization and at the same time allows for simulated time rather than real time
void UpdatePegasusTime();

//! Get the current system time
//! \note Each call returns the same value, until the next call to \a UpdateTime()
//! \warning This value is not guaranteed to start at 0.0 when the application starts
//! \return System time in seconds
double GetPegasusTime();


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_TIME_H
