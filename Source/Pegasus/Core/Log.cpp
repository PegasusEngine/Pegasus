/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Log.cpp
//! \author Kevin Boulanger
//! \date   04th July 2013
//! \brief  Log manager, for debug output

#include "Pegasus/Core/Log.h"

#if PEGASUS_ENABLE_LOG

#include <stdio.h>
#include <stdarg.h>

namespace Pegasus {
namespace Core {

//! Maximum size of the buffer containing one log message
static const size_t LOGARGS_BUFFER_SIZE = 1024; 

//----------------------------------------------------------------------------------------

LogManager::LogManager()
:   mHandler(nullptr)
{
}

//----------------------------------------------------------------------------------------

LogManager::~LogManager()
{
}

//----------------------------------------------------------------------------------------

void LogManager::RegisterHandler(Handler handler)
{
    mHandler = handler;
}

//----------------------------------------------------------------------------------------

void LogManager::UnregisterHandler()
{
    mHandler = nullptr;
}

//----------------------------------------------------------------------------------------

void LogManager::Log(LogChannel logChannel, const char * msgStr, ...)
{
    if (mHandler != nullptr)
    {
        // Handler defined. Call it.

        // Format the input string with the extra parameters if there are any
        char * formattedString = nullptr;
        if (msgStr != nullptr)
        {
            static char buffer[LOGARGS_BUFFER_SIZE];
            va_list args;
            va_start(args, msgStr);
            vsnprintf_s(buffer, LOGARGS_BUFFER_SIZE, LOGARGS_BUFFER_SIZE - 1, msgStr, args);
            va_end(args);

            formattedString = buffer;
        }

        mHandler(logChannel, formattedString);
    }
    else
    {
        // Handler undefined. Throw an assertion error
        PG_FAILSTR("No log message handler has been defined. Call LogManager::GetInstance().RegisterHandler(handler) at least once");
    }
}


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_LOG
