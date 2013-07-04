/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Assertion.cpp
//! \author	Kevin Boulanger
//! \date	24th June 2013
//! \brief	Assertion test macros and manager

#include "Pegasus/Core/Assertion.h"

#if PEGASUS_ENABLE_ASSERT


namespace Pegasus {
namespace Core {


AssertionManager::AssertionManager()
:   mHandler(nullptr)
{
}

//----------------------------------------------------------------------------------------

AssertionManager::~AssertionManager()
{
}

//----------------------------------------------------------------------------------------

void AssertionManager::RegisterHandler(Handler handler)
{
    mHandler = handler;
}

//----------------------------------------------------------------------------------------

void AssertionManager::UnregisterHandler()
{
    mHandler = nullptr;
}

//----------------------------------------------------------------------------------------

void AssertionManager::AssertionError(const char * testStr,
                                      const char * fileStr,
                                      int line,
                                      const char * msgStr)
{
    if (mHandler)
    {
        // Handler defined. Call it.
        PG_ASSERTSTR(testStr != nullptr, "The test string has to be defined for an assertion error");
        PG_ASSERTSTR(fileStr != nullptr, "The file name string has to be defined for an assertion error");
        PG_ASSERTSTR(line >= 0, "Invalid line number for an assertion error");
        mHandler(testStr, fileStr, line, msgStr);
    }
    else
    {
        // Handler undefined. Use a debug break, or an actual crash if unavailable
#if PEGASUS_COMPILER_MSVC
        __debugbreak();
#else
        static volatile char * crashMe = 0;
        *crashMe = 0;
#endif
    }
}


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_ASSERT
