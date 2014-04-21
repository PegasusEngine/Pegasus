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

#include "Pegasus/Shader/Shared/IEventListener.h"
#include "Pegasus/Core/Shared/IoErrors.h"

// generic definition of visitor callback
#define DECLARE_VISITOR virtual void Visit(IUserData * userData, IEventListener * listener) {listener->OnEvent(userData, *this);}

namespace Pegasus
{
    namespace Shader
    {
        class IEventListener;
        class ShaderStage;
    }
}

namespace Pegasus
{
namespace Shader
{
    //! data to be filled by the editor
    class IUserData
    {
    public:
        //! empty constructor
        IUserData(){}

        //! empty destructor
        virtual ~IUserData(){}
    };

    //! generic event
    class Event
    {
    public:
        //! generic event constructor
        explicit Event() {}

        //! generic destructor
        virtual ~Event(){};

        //! Visitor callback, meant to dispatch to the listener correctly
        virtual void Visit(IUserData * userData, IEventListener * listener) = 0;

    private:
        //no copy constructor
        explicit Event(const Event& other);
        Event& operator= (const Event& other);
        IUserData * mUserData;
    };

    //! loading event: fired when shader loading is finished
    class ShaderLoadedEvent : public Event
    {
    public:
        //! internal visitor callback
        DECLARE_VISITOR;

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
    class CompilationEvent : public Event
    {
    public:
        //! internal visitor callback
        DECLARE_VISITOR;

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
    class CompilationNotification : public Event
    {
    public:
        //! internal visitor callback
        DECLARE_VISITOR;

        //! type of notification: warning or error
        enum Type
        {
            COMPILATION_ERROR,
            COMPILATION_WARNING
        };

        //! constructor
        CompilationNotification(Type t, int row, int column, const char * description)
        : mType(t), mColumn(column), mRow(row), mDescription(description)
        {}

        //! destructor
        virtual ~CompilationNotification(){}

        //! gets the column where the error / warning occurred
        //! \returns zero based int for the column number
        int GetColumn() const { return mColumn; }

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
    class LinkingEvent : public Event
    {
    public:
        //! internal visitor callback
        DECLARE_VISITOR;
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
    class FileOperationEvent : public Event
    {
    public:
        //! internal visitor callback
        DECLARE_VISITOR;
        enum Type
        {
            WRONG_EXTENSION, //wrong extension
            IO_ERROR //an IO error, more details in the GetIoError() function
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
}
}

#endif
