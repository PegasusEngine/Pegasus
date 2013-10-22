/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EventDispatcher.h
//! \author	Kleber Garcia
//! \date	16th October 2013
//! \brief	Event dispatcher class

#ifndef PEGASUS_EVENTDISPATCHER_H 
#define PEGASUS_EVENTDISPATCHER_H

//use the event system macros only if we are in dev mode
#if PEGASUS_SHADER_USE_EDIT_EVENTS

#define SHADEREVENT_WRONG_FILE_FORMAT(file, msg) \
    { \
    Pegasus::Shader::FileOperationEvent e(\
        Pegasus::Shader::FileOperationEvent::WRONG_EXTENSION, \
        Pegasus::Io::ERR_NONE, file, msg); \
    DispatchEvent(e); \
    } 

#define SHADEREVENT_IO_ERROR(ioError, file, msg) \
    { \
    Pegasus::Shader::FileOperationEvent e(\
        Pegasus::Shader::FileOperationEvent::IO_ERROR, \
        ioError, file, msg); \
    DispatchEvent(e); \
    }

#define SHADEREVENT_LOADED(src, srcSize) \
    { \
    Pegasus::Shader::ShaderLoadedEvent e(src, srcSize); \
    DispatchEvent(e); \
    }

#define SHADEREVENT_COMPILATION_FAIL(log) \
    { \
    Pegasus::Shader::CompilationEvent e(false, log);\
    DispatchEvent(e); \
    }

#define SHADEREVENT_COMPILATION_SUCCESS \
    { \
    Pegasus::Shader::CompilationEvent e(true, "" );\
    DispatchEvent(e); \
    }

#define SHADEREVENT_COMPILATION_ERROR(row, column, description)\
    {\
    Pegasus::Shader::CompilationNotification e(\
        Pegasus::Shader::CompilationNotification::ERROR, row, column, description);\
    DispatchEvent(e); \
    }

#define SHADEREVENT_LINKING_SUCCESS \
    { \
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::LINKING_SUCCESS, "" );\
    DispatchEvent(e); \
    }

#define SHADEREVENT_LINKING_FAIL(log) \
    { \
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::LINKING_FAIL, log);\
    DispatchEvent(e); \
    }

#define SHADEREVENT_LINKING_INCOMPLETE \
    {\
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::INCOMPLETE_STAGES_FAIL, "");\
    DispatchEvent(e); \
    }

#define SHADEREVENT_ATTACHMENT_FAIL(dispatcher) \
    {\
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::SHADER_ATTACHMENT_FAIL, "");\
    dispatcher->DispatchEvent(e); \
    }

#else

//empty macros
#define SHADEREVENT_WRONG_FILE_FORMAT(file, msg)
#define SHADEREVENT_IO_ERROR(ioError, file, msg)
#define SHADEREVENT_LOADED(src, srcSize)
#define SHADEREVENT_COMPILATION_FAIL(log)
#define SHADEREVENT_COMPILATION_SUCCESS 
#define SHADEREVENT_COMPILATION_ERROR(row, column, description)
#define SHADEREVENT_LINKING_SUCCESS 
#define SHADEREVENT_LINKING_FAIL(log)
#define SHADEREVENT_LINKING_INCOMPLETE
#define SHADEREVENT_ATTACHMENT_FAIL(dispatcher)


#endif

namespace Pegasus
{
    namespace Shader
    {
        class IUserData;
        class IEventListener;
        class Event;
    }
}

namespace Pegasus
{
namespace Shader
{
    class EventDispatcher
    {
    public:
        EventDispatcher();
        virtual ~EventDispatcher();
        
        void SetUserData(IUserData * userData) {mUserData = userData;}
        void SetEventListener(IEventListener * listener) { mEventListener = listener; }
        void ClearEventListener() { mEventListener = nullptr; }
        void DispatchEvent(Event& event);

        IUserData * GetUserData() { return mUserData; };

    private:
        //no copy constructor.
        explicit EventDispatcher(const EventDispatcher& other);
        EventDispatcher& operator=(const EventDispatcher& other);

        IEventListener * mEventListener;
        IUserData * mUserData;
    };
}
}

#endif
