/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Log.cpp
//! \author	Kevin Boulanger
//! \date	04th July 2013
//! \brief	Log manager, for debug output

#include "Pegasus/Core/Log.h"

#if PEGASUS_ENABLE_LOG


namespace Pegasus {
namespace Core {


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

void LogManager::Log(LogChannel channel, const char * msgStr)
{
    if (mHandler)
    {
        // Handler defined. Call it.
        mHandler(channel, msgStr);
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
