/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderEvent.h
//! \author	Kleber Garcia
//! \date	16th October 2013
//! \brief	Event classes for shaders

#ifndef PEGASUS_SHADEREVENT_H 
#define PEGASUS_SHADEREVENT_H

#include "Pegasus/Core/Shared/IoErrors.h"
#include "Pegasus/Graph/Shared/GraphEventDefs.h"

namespace Pegasus
{
namespace Shader
{
    //! loading event: fired when shader loading is finished
    class ShaderLoadedEvent
    {
    public:
        //! constructor
        ShaderLoadedEvent(const char * src, int length)
        : mSrc(src), mBufferLength(length)
        {}

        //! destructor
        virtual ~ShaderLoadedEvent(){}
        
        //! Gets the source of a shader once loaded
        const char * GetShaderSrc() const { return mSrc; }

        //! returns the src buffer length
        int GetSrcBufferLength() const { return mBufferLength; }

    private:
        const char * mSrc;
        int   mBufferLength;
    };

    //! compilation event: fired when compilation is finished
    class CompilationEvent 
    {
    public:
        //! constructor
        CompilationEvent(bool success, const char * log)
        : mSuccess(success), mLogString(log)
        {}

        //! destructor
        virtual ~CompilationEvent() {}

        //! returns the log string cached
        const char * GetLogString() const { return mLogString; }

        //! returns success whether shader compilation succeded or failed
        bool IsSuccess() const { return mSuccess; }
    private:
        const char * mLogString;
        bool mSuccess;
    };

    //! a particular compilation issue on a shader, to be used by a shader IDE
    class CompilationNotification 
    {
    public:
        //! type of notification: warning or error
        enum Type
        {
            COMPILATION_ERROR,
            COMPILATION_WARNING,
            COMPILATION_BEGIN
        };

        //! constructor
        CompilationNotification(Type t, int row, const char * description)
        : mType(t), mRow(row), mDescription(description)
        {}

        //! destructor
        virtual ~CompilationNotification(){}

        //! gets the row where the error / warning occurred
        //! \returns zero based int for the row number
        int GetRow() const    { return mRow;    }

        //! gets the type warning / error type
        Type GetType() const  { return mType;   }

        //! text with description
        const char * GetDescription() const { return mDescription; }
    private:
        Type mType;
        int mColumn;
        int mRow;
        const char * mDescription;
    
    };


    //! fired when linking notification occurs
    class LinkingEvent
    {
    public:
        enum Type
        {
            LINKING_SUCCESS,
            LINKING_FAIL,
            INCOMPLETE_STAGES_FAIL,
            SHADER_ATTACHMENT_FAIL
        };

        //! constructor
        LinkingEvent(Type eventType, const char * log)
        : mEventType(eventType), mLog(log) {}

        //! destructor
        virtual ~LinkingEvent(){}

        //! gets the type of event compilation type
        Type GetEventType() const { return mEventType; }

        //! gets the log type
        const char * GetLog() const { return mLog; }
    private:
        Type mEventType;
        const char * mLog;
    };

    //! fired when file loading operation occurs
    class FileOperationEvent
    {
    public:
        enum Type
        {
            WRONG_EXTENSION, //wrong extension
            IO_ERROR, //an IO error, more details in the GetIoError() function
            IO_FILE_SAVE_SUCCESS, //success signal when a file has been saved
            IO_FILE_SAVE_ERROR, //success signal when a file has been saved
        };

        //! constructor
        FileOperationEvent(
            Type t, 
            Pegasus::Io::IoError ioError, 
            const char * filepath, 
            const char * message)
            :
        mMessage(message), mFilePath(filepath), mType(t), mIoError(ioError)
        {
        }

        //! destructor
        virtual ~FileOperationEvent(){}

        //! gets the event type
        Type GetType() const { return mType; }

        //! gets the message string 
        const char * GetMessage() const { return mMessage; }

        //! which file?
        const char * GetFilePath() const { return mFilePath; }

        //! gets the type of io error
        Pegasus::Io::IoError GetIoError() const { return mIoError; }

    private:
        Type mType;
        Pegasus::Io::IoError mIoError;
        const char * mMessage;
        const char * mFilePath;
        
    };

    GRAPH_EVENT_BEGIN_REGISTRY (IShaderEventListener)
        GRAPH_EVENT_REGISTER (ShaderLoadedEvent)
        GRAPH_EVENT_REGISTER (CompilationEvent)
        GRAPH_EVENT_REGISTER (CompilationNotification)
        GRAPH_EVENT_REGISTER (LinkingEvent)
        GRAPH_EVENT_REGISTER (FileOperationEvent)
    GRAPH_EVENT_END_REGISTRY

}


}

#endif
