/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EventDefs.h
//! \author	Kleber Garcia
//! \date	4th May 2014
//! \brief	Definitions for Events. This file contains utilities to create custom
//!         events to communicate to an editor. Only enabled in dev mode

#ifndef PEGASUS_CORE_EVENT_DEFS_H
#define PEGASUS_CORE_EVENT_DEFS_H

#if PEGASUS_USE_EVENTS

namespace Pegasus {
namespace Core {
    //! generic class, used by the user to pass event data around
    class IEventUserData 
    {
    public: 
        IEventUserData(){}
        virtual ~IEventUserData(){}
    };

    //! \brief internal dispatching function, for internal use only
    template<class E, class L, class S>
    void Internal_DispatchEvent(S* s, E& e, L* l) 
    {
        if (l != nullptr)
        {
            l->OnEvent(s->GetEventUserData(), e);
        }
    }

    //! \brief internal init user data function, for internal use only
    template<class ProxyType, class L>
    void Internal_InitUserData(ProxyType* s, const char* name, L* l) 
    {
        if (l != nullptr)
        {
            l->OnInitUserData(s, name);
        }
    }

    //! \brief internal destroy user data function, for internal use only
    template<class ProxyType, class L>
    void Internal_DestroyUserData(ProxyType* s, const char* name, L* l) 
    {
        if (l != nullptr)
        {
            l->OnDestroyUserData(s, name);
        }
    }
}
}

//! \brief Pass the name of the event listener on the registry
#define PEGASUS_EVENT_BEGIN_REGISTRY(__listener_name) class __listener_name { public: __listener_name(){} virtual ~__listener_name(){}

//! \brief Pass the name of the event struct to register
#define PEGASUS_EVENT_REGISTER(__event) virtual void OnEvent(Pegasus::Core::IEventUserData * d, __event& e) = 0;

//! \brief Pass the proxies that will get user data injected
#define PEGASUS_EVENT_REGISTER_PROXY(__proxy) virtual void OnInitUserData(__proxy* proxy, const char* name) = 0; \
                                            virtual void OnDestroyUserData(__proxy* proxy, const char* name) = 0;

//! \brief call at the end of the event registry once you are done
#define PEGASUS_EVENT_END_REGISTRY };

//! \brief declare this statement at the very beginning of your node that will send events. Make sure to pass the event listener type declared in BEGIN_EVENT_REGISTRY
#define PEGASUS_EVENT_DECLARE_DISPATCHER(_listener_type) public: \
    Pegasus::Core::IEventUserData * GetEventUserData() { return __mUserData;} \
    void SetEventUserData(Pegasus::Core::IEventUserData * userData)  { __mUserData = userData; InvalidateData(); } \
    _listener_type * GetEventListener() { return __mEventListener; } \
    void SetEventListener(_listener_type * l) { __mEventListener = l; } \
    private: _listener_type * __mEventListener; Pegasus::Core::IEventUserData * __mUserData; public:

//! \brief use this init to inject user data into the proxy
#define PEGASUS_EVENT_INIT_USER_DATA(sender, name, listener) Pegasus::Core::Internal_InitUserData(sender, name, listener)

//! \brief use this destory to destroy user data into the proxy
#define PEGASUS_EVENT_DESTROY_USER_DATA(sender, name, listener) Pegasus::Core::Internal_DestroyUserData(sender, name, listener)

//! \brief init macro, call at the constructors of the Node classes sending events
#define PEGASUS_EVENT_INIT_DISPATCHER SetEventListener(nullptr);SetEventUserData(nullptr);

//! \brief call when an event must be dispatched if a listener is attached.
//! \param the sender pointer. "this: pointer usually, or the node graph holding it
//! \param the event type to be dispatched
//! \param the arguments that match the constructor of the event class
#define PEGASUS_EVENT_DISPATCH(sender, event, ...) Pegasus::Core::Internal_DispatchEvent(sender, event(__VA_ARGS__), sender->GetEventListener())

#else
// for events disabled, empty declarations

#define PEGASUS_EVENT_BEGIN_REGISTRY(__listener_name) 
#define PEGASUS_EVENT_REGISTER_PROXY(__proxy)
#define PEGASUS_EVENT_REGISTER(__event) 
#define PEGASUS_EVENT_END_REGISTRY
#define PEGASUS_EVENT_DECLARE_DISPATCHER(_listener_type)
#define PEGASUS_EVENT_INIT_DISPATCHER 
#define PEGASUS_EVENT_DISPATCH(sender, event, ...)
#define PEGASUS_EVENT_INIT_USER_DATA(sender, name, listener) 
#define PEGASUS_EVENT_DESTROY_USER_DATA(sender, name, listener)

#endif //PEGASUS_USE_PEGASUS_EVENTS

#endif //PEGASUS_PEGASUS_EVENT_DEFS_H

