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
    //to be filled by the user
    class IUserData
    {
    public:
        IUserData(){}
        ~IUserData(){}
    };

    class Event
    {
    public:
        explicit Event() {}
        virtual ~Event(){};
        virtual void Visit(IUserData * userData, IEventListener * listener) = 0;
    private:
        //no copy constructor
        explicit Event(const Event& other);
        Event& operator= (const Event& other);
        IUserData * mUserData;
    };

    class ShaderLoadedEvent : public Event
    {
    public:
        DECLARE_VISITOR;
        ShaderLoadedEvent(const char * src, int length)
        : mSrc(src), mBufferLength(length)
        {}
        virtual ~ShaderLoadedEvent(){}
        const char * GetShaderSrc() const { return mSrc; }
        int GetSrcBufferLength() const { return mBufferLength; }

    private:
        const char * mSrc;
        int   mBufferLength;
    };

    class CompilationEvent : public Event
    {
    public:
        DECLARE_VISITOR;
        CompilationEvent(bool success, const char * log)
        : mSuccess(success), mLogString(log)
        {}
        virtual ~CompilationEvent() {}
        const char * GetLogString() const { return mLogString; }
        bool IsSuccess() const { return mSuccess; }
    private:
        const char * mLogString;
        bool mSuccess;
    };

    class CompilationNotification : public Event
    {
    public:
        DECLARE_VISITOR;
        enum Type
        {
            COMPILATION_ERROR,
            COMPILATION_WARNING
        };
        CompilationNotification(Type t, int row, int column, const char * description)
        : mType(t), mColumn(column), mRow(row), mDescription(description)
        {}
        ~CompilationNotification(){}
        int GetColumn() const { return mColumn; }
        int GetRow() const    { return mRow;    }
        Type GetType() const  { return mType;   }
        const char * GetDescription() const { mDescription; }
    private:
        Type mType;
        int mColumn;
        int mRow;
        const char * mDescription;
    
    };

    class LinkingEvent : public Event
    {
    public:
        DECLARE_VISITOR;
        enum Type
        {
            LINKING_SUCCESS,
            LINKING_FAIL,
            INCOMPLETE_STAGES_FAIL,
            SHADER_ATTACHMENT_FAIL
        };
        LinkingEvent(Type eventType, const char * log)
        : mEventType(eventType), mLog(log) {}
        virtual ~LinkingEvent(){}
        Type GetEventType() const { return mEventType; }
        const char * GetLog() const { return mLog; }
    private:
        Type mEventType;
        const char * mLog;
    };

    class FileOperationEvent : public Event
    {
    public:
        DECLARE_VISITOR;
        enum Type
        {
            WRONG_EXTENSION, //wrong extension
            IO_ERROR //an IO error, more details in the GetIoError() function
        };
        FileOperationEvent(
            Type t, 
            Pegasus::Io::IoError ioError, 
            const char * filepath, 
            const char * message)
            :
        mMessage(message), mFilePath(filepath), mType(t), mIoError(ioError)
        {
        }

        virtual ~FileOperationEvent(){}
        Type GetType() const { return mType; }
        const char * GetMessage() const { return mMessage; }
        const char * GetFilePath() const { return mFilePath; }
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
