/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Assertion.cpp
//! \author Kevin Boulanger
//! \date   24th June 2013
//! \brief  Assertion test macros and manager

#include "Pegasus/Core/Assertion.h"

#if PEGASUS_ENABLE_ASSERT

#include <stdio.h>
#include <stdarg.h>

namespace Pegasus {
namespace Core {

//! Maximum size of the buffer containing one assertion error message
static const size_t ASSERTIONERRORARGS_BUFFER_SIZE = 1024; 

//----------------------------------------------------------------------------------------

AssertionManager::AssertionManager()
:   mHandler(nullptr),
    mAssertionBeingHandled(false)
{
}

//----------------------------------------------------------------------------------------

AssertionManager::~AssertionManager()
{
}

//----------------------------------------------------------------------------------------

void AssertionManager::RegisterHandler(AssertionHandlerFunc handler)
{
    mHandler = handler;
}

//----------------------------------------------------------------------------------------

void AssertionManager::UnregisterHandler()
{
    mHandler = nullptr;
}

//----------------------------------------------------------------------------------------

AssertReturnCode AssertionManager::AssertionError(const char * testStr,
                                                  const char * fileStr,
                                                  int line,
                                                  const char * msgStr, ...)
{
    if (mHandler != nullptr)
    {
        // Handler defined. Call it.
        PG_ASSERTSTR(testStr != nullptr, "The test string has to be defined for an assertion error");
        PG_ASSERTSTR(fileStr != nullptr, "The file name string has to be defined for an assertion error");
        PG_ASSERTSTR(line >= 0, "Invalid line number for an assertion error");

        // If a message string is present, format it with the extra parameters if there are any
        char * formattedString = nullptr;
        if (msgStr != nullptr)
        {
            static char buffer[ASSERTIONERRORARGS_BUFFER_SIZE];
            va_list args;
            va_start(args, msgStr);
            vsnprintf_s(buffer, ASSERTIONERRORARGS_BUFFER_SIZE, ASSERTIONERRORARGS_BUFFER_SIZE - 1, msgStr, args);
            va_end(args);

            formattedString = buffer;
        }

        // Call the registered assertion handler
        mAssertionBeingHandled = true;
        AssertReturnCode returnCode = mHandler(testStr, fileStr, line, formattedString);
        mAssertionBeingHandled = false;

        return returnCode;
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

        return ASSERTION_BREAK;
    }
}


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_ASSERT
