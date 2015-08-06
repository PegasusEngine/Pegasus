/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Log.h
//! \author	Karolyn Boulanger
//! \date	22th August 2013
//! \brief	Log manager, for debug output

#ifndef EDITOR_LOG_H
#define EDITOR_LOG_H

#include "Editor.h"

#include "Pegasus/Core/Shared/LogChannel.h"

#include <QObject>


//! Macro to send a formatted message to the editor log channel
//! \param str String of the message to log, with the same formatting syntax as printf()
//! \warning The number of parameters following str must match the list of formatting
//!          strings inside msgStr.
//! \warning Do not use QString directly as a parameter, call .toLatin1().constData() first.
#define ED_LOG(str, ...)        { Editor::GetInstance().GetLogManager().Log('EDIT', str, __VA_ARGS__); }

//----------------------------------------------------------------------------------------

//! Log manager (singleton) that outputs text to the console dock widget
class LogManager : public QObject
{
    Q_OBJECT

public:

    LogManager(Editor * parent);
    virtual ~LogManager();

    //! Send a formatted message to the console dock widget, for a specific log channel.
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log, with the same formatting syntax as printf()
    //! \warning The number of parameters following msgStr must match the list of formatting
    //!          strings inside msgStr.
    //! \warning Do not use QString directly as a parameter, call .toLatin1().constData() first.
    void Log(Pegasus::Core::LogChannel logChannel, const char * msgStr, ...);

    //! Send an unformatted message to the console dock widget, for a specific log channel.
    //! \param logChannel Log channel that receives the message
    //! \param msgStr String of the message to log
    void LogNoFormat(Pegasus::Core::LogChannel logChannel, const QString & msgStr);

private:

    //! Pointer to the parent Editor object
    Editor * mEditor;
};


#endif  // EDITOR_LOG_H
