/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LogChannel.h
//! \author David Worsham
//! \date   03 Nov 2013
//! \brief  Log channel type declaration.

#ifndef PEGASUS_CORE_LOGCHANNEL_H
#define PEGASUS_CORE_LOGCHANNEL_H

namespace Pegasus {
namespace Core {

//! Type definition of a log channel.
//! A channel name is represented with 4 uppercase letter inside single quotes, such as 'WARN'.
//! Use underscores to replace spaces when the name is less than 4 letters long.
typedef unsigned long LogChannel;

//! Set of predefined log channels that can be used throughout the engine
static const LogChannel sLogChannels[] = {
    'CRIT',     // Critical errors, typically resulting in a crash or instability
    'ERR_',     // General errors that can be recovered.
    'CERR',     // Compilation Errors, for codes / scripts compiled internally.
    'WARN',     // General warnings.
    'ASRT',     // Assertions

    'EDIT',     // Editor
    'TEMP',     // Temporary channel for debugging, only for the SB branch
    
    'APPL',     // Application global information
    'WNDW',     // Window management
    'OGL_',     // OpenGL specific
 
    'FILE',     // File management
    'ASST',     // Asset management

    'TMLN',     // Timeline info
    'TXTR',     // Texture (generation)
    'MESH',     // Mesh (generation)
    'SHDR',     // Shader (generation)
    'SOUN',     // Sound info
    'MUSC',     // Music info
};

//! Number of defined channels
static const unsigned int NUM_LOG_CHANNELS = sizeof(sLogChannels) / sizeof(LogChannel);

//----------------------------------------------------------------------------------------

//! Callback function declaration.
//! One function with this type needs to be declared in the user application
//! to handle log messages.
//! \param logChannel Log channel that receives the message
//! \param msgStr String of the message to log
typedef void (* LogHandlerFunc)(LogChannel logChannel, const char * msgStr);


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_LOGCHANNEL_H
