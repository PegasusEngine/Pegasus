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
//! \param str String of the message
#define PG_LOG(channel, str)    { Pegasus::Core::LogManager::GetInstance().Log(channel, str); }

//! \todo Add support for formatted strings

#else

#define PG_LOG(channel, str)

#endif  // PEGASUS_ENABLE_LOG

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_LOG

#include "Pegasus/Core/Singleton.h"

namespace Pegasus {
namespace Core {
    

//! Type definition of a log channel.
//! A channel name is represented with 4 uppercase letter inside single quotes, such as 'WARN'.
//! Use underscores to replace spaces when the name is less than 4 letters long.
typedef unsigned long LogChannel;

//! Set of predefined log channels that can be used throughout the engine
static const LogChannel sLogChannels[] = {
    'CRIT',     // Critical errors, typically resulting in a crash or instability
    'ERR_',     // Error that can be recovered
    'WARN',     // Warnings
    'ASRT',     // Assertions

    'EDIT',     // Editor
    'TEMP',     // Temporary channel for debugging, only for the SB branch
    
    'APPL',     // Application global information
    'WNDW',     // Window management
    'OGL_',     // OpenGL specific
    'FILE',     // File management
    'ASST',     // Asset management
    'TMLN',     // Timeline info
    'MUSC',     // Music info
    'SHDR',     // Shader (compilation, info, error)
};

//! Number of defined channels
static const unsigned int NUM_LOG_CHANNELS = sizeof(sLogChannels) / sizeof(LogChannel);


//! Log manager (singleton) that redirects the macros to the log handler,
//! for debug messages
class LogManager : public AutoSingleton<LogManager>
{
public:

    LogManager();
    ~LogManager();

    //! Callback function declaration.
    //! One function with this type needs to be declared in the user application
    //! to handle log messages.
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log
    typedef void (* Handler)(LogChannel logChannel, const char * msgStr);

    //! Register the log message handler
    //! \warning To be called at least once by the user application. Otherwise,
    //!          an assertion error will happen when trying to send a log message
    //! \param handler Function pointer of the log message handler (!= nullptr)
    void RegisterHandler(Handler handler);

    //! Unregister the log message handler if defined
    void UnregisterHandler();


    //! Send a message to the log output, for a specific channel.
    //! The handler registered with \a RegisterHandle is called with the provided parameters
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log
    void Log(LogChannel logChannel, const char * msgStr);

private:

    //! Function pointer of the log message handler, nullptr by default
    Handler mHandler;
};


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_LOG

#endif  // PEGASUS_CORE_LOG_H
