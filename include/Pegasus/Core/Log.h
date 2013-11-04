/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Log.h
//! \author	Kevin Boulanger
//! \date	04th July 2013
//! \brief	Log manager, for debug output

#ifndef PEGASUS_CORE_LOG_H
#define PEGASUS_CORE_LOG_H

#include "Pegasus/Preprocessor.h"

#if PEGASUS_ENABLE_LOG

//! Macro to send a message to a log channel
//! \param channel LogChannel to send to, refer the list below (\a sLogChannels)
//! \param str String of the message to log, with the same formatting syntax as printf()
//! \warning The number of parameters following str must match the list of formatting
//!          strings inside msgStr.
#define PG_LOG(channel, str, ...)   { Pegasus::Core::LogManager::GetInstance()->Log(channel, str, __VA_ARGS__); }
#else
#define PG_LOG(channel, str, ...)
#endif  // PEGASUS_ENABLE_LOG

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_LOG
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Singleton.h"

namespace Pegasus {
namespace Core {

//! Log manager (singleton) that redirects the macros to the log handler,
//! for debug messages
class LogManager : public Singleton<LogManager>
{
public:
    //! Constructor
    LogManager();

    //! Destructor
    virtual ~LogManager();


    //! Register the log message handler
    //! \warning To be called at least once by the user application. Otherwise,
    //!          an assertion error will happen when trying to send a log message
    //! \param handler Function pointer of the log message handler (!= nullptr)
    void RegisterHandler(LogHandlerFunc handler);

    //! Unregister the log message handler if defined
    void UnregisterHandler();


    //! Send a formatted message to the log output, for a specific channel.
    //! The handler registered with \a RegisterHandle is called with the provided parameters
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log, with the same formatting syntax as printf()
    //! \warning The number of parameters following msgStr must match the list of formatting
    //!          strings inside msgStr.
    void Log(LogChannel logChannel, const char * msgStr, ...);

private:

    //! Function pointer of the log message handler, nullptr by default
    LogHandlerFunc mHandler;
};


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_LOG

#endif  // PEGASUS_CORE_LOG_H
