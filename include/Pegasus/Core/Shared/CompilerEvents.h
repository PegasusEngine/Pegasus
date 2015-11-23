/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CompilerEvents.h
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Event classes for a generic compiler

#ifndef PEGASUS_COMPILEREVENTS_H 
#define PEGASUS_COMPILEREVENTS_H

#include "Pegasus/Core/Shared/IoErrors.h"
#include "Pegasus/Core/Shared/EventDefs.h"

namespace Pegasus
{
namespace Core
{
    class IBasicSourceProxy;

namespace CompilerEvents
{
    //! loading event: fired when source loading is finished
    class SourceLoadedEvent
    {
    public:
        //! constructor
        SourceLoadedEvent(const char * src, int length)
        : mSrc(src), mBufferLength(length)
        {}

        //! destructor
        virtual ~SourceLoadedEvent(){}
        
        //! Gets the source of  once loaded
        const char * GetSrc() const { return mSrc; }

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

        //! returns success whether compilation succeded or failed
        bool IsSuccess() const { return mSuccess; }
    private:
        const char * mLogString;
        bool mSuccess;
    };

    //! a particular compilation issue , to be used by a code IDE
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

    PEGASUS_EVENT_BEGIN_REGISTRY (ICompilerEventListener)
        PEGASUS_EVENT_REGISTER_PROXY(IBasicSourceProxy)
        PEGASUS_EVENT_REGISTER (SourceLoadedEvent)
        PEGASUS_EVENT_REGISTER (CompilationEvent)
        PEGASUS_EVENT_REGISTER (CompilationNotification)
        PEGASUS_EVENT_REGISTER (LinkingEvent)
    PEGASUS_EVENT_END_REGISTRY

} //namespace CompilationEvents
} //namespace Core
} //namespace Pegasus

#endif
