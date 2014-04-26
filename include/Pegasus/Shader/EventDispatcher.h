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

//! dispatches a shader event with wrong file format
#define SHADEREVENT_WRONG_FILE_FORMAT(file, msg) \
    { \
    Pegasus::Shader::FileOperationEvent e(\
        Pegasus::Shader::FileOperationEvent::WRONG_EXTENSION, \
        Pegasus::Io::ERR_NONE, file, msg); \
    DispatchEvent(e); \
    } 

//! dispatches a shader event with an IO error
#define SHADEREVENT_IO_ERROR(ioError, file, msg) \
    { \
    Pegasus::Shader::FileOperationEvent e(\
        Pegasus::Shader::FileOperationEvent::IO_ERROR, \
        ioError, file, msg); \
    DispatchEvent(e); \
    }

//! dispatches a shader event when a shader has been loaded successfuly from a file
#define SHADEREVENT_LOADED(src, srcSize) \
    { \
    Pegasus::Shader::ShaderLoadedEvent e(src, srcSize); \
    DispatchEvent(e); \
    }

//! dispatches a shader event triggering a failed compilation
#define SHADEREVENT_COMPILATION_FAIL(dispatcher, log) \
    { \
    Pegasus::Shader::CompilationEvent e(false, log);\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a successful shader compilation event
#define SHADEREVENT_COMPILATION_SUCCESS(dispatcher) \
    { \
    Pegasus::Shader::CompilationEvent e(true, "" );\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a compilation error
#define SHADEREVENT_COMPILATION_ERROR(dispatcher, row, description)\
    {\
    Pegasus::Shader::CompilationNotification e(\
        Pegasus::Shader::CompilationNotification::COMPILATION_ERROR, row, description);\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a compilation begin event
#define SHADEREVENT_COMPILATION_BEGIN(dispatcher)\
    {\
    Pegasus::Shader::CompilationNotification e(\
        Pegasus::Shader::CompilationNotification::COMPILATION_BEGIN, 0, "");\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a linking error
#define SHADEREVENT_LINKING_SUCCESS(dispatcher) \
    { \
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::LINKING_SUCCESS, "" );\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a linking fail error
#define SHADEREVENT_LINKING_FAIL(dispatcher, log) \
    { \
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::LINKING_FAIL, log);\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a linking completion
#define SHADEREVENT_LINKING_INCOMPLETE(dispatcher) \
    {\
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::INCOMPLETE_STAGES_FAIL, "");\
    dispatcher->DispatchEvent(e); \
    }

//! dispatches a shader attachment failure when linking
#define SHADEREVENT_ATTACHMENT_FAIL(dispatcher) \
    {\
    Pegasus::Shader::LinkingEvent e(Pegasus::Shader::LinkingEvent::SHADER_ATTACHMENT_FAIL, "");\
    dispatcher->DispatchEvent(e); \
    }

#else

//! empty event macros. Event system has been disabled
#define SHADEREVENT_WRONG_FILE_FORMAT(file, msg)
#define SHADEREVENT_IO_ERROR(ioError, file, msg)
#define SHADEREVENT_LOADED(src, srcSize)
#define SHADEREVENT_COMPILATION_FAIL(dispatcher, log)
#define SHADEREVENT_COMPILATION_SUCCESS(dispatcher)
#define SHADEREVENT_COMPILATION_ERROR(dispatcher, row, description)
#define SHADEREVENT_COMPILATION_BEGIN(dispatcher)
#define SHADEREVENT_LINKING_SUCCESS(dispatcher)
#define SHADEREVENT_LINKING_FAIL(dispatcher, log)
#define SHADEREVENT_LINKING_INCOMPLETE(dispatcher)
#define SHADEREVENT_ATTACHMENT_FAIL(dispatcher)


#endif

//! fwd declarations
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
    //! Class that dispatches a shader event message
    class EventDispatcher
    {
    public:
        EventDispatcher();
        virtual ~EventDispatcher();
        
        //! sets a blank IUserData structure. To be used by the editor so it can be passed
        //! around. Any metadata that the editor application would want to add.
        //! \param userData the user data to set
        void SetUserData(IUserData * userData) {mUserData = userData;}
        
        //! Sets an event listener for this dispatcher. The event listener will act as the major
        //! callback entry point for any shader event.
        //! \param listener the listener interface
        void SetEventListener(IEventListener * listener) { mEventListener = listener; }

        //! clears the current listener
        void ClearEventListener() { mEventListener = nullptr; }

        //! Dispatches a generic event to the listener
        //! \param event the event to dispatch
        void DispatchEvent(Event& event);

        //! Returns the user data set by the default element
        IUserData * GetUserData() { return mUserData; };

    private:
        //no copy constructor.
        explicit EventDispatcher(const EventDispatcher& other);
        EventDispatcher& operator=(const EventDispatcher& other);

        //! pointer to the listener
        IEventListener * mEventListener;

        //! pointer to the user data
        IUserData * mUserData;
    };
}
}

#endif
