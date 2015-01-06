/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEventDefs.h
//! \author	Kleber Garcia
//! \date	4th May 2014
//! \brief	Definitions for Node Events. This file contains utilities to create custom node
//!         events to communicate to an editor. Only enabled in dev mode
#ifndef PEGASUS_GRAPH_EVENT_DEFS_H
#define PEGASUS_GRAPH_EVENT_DEFS_H

#if PEGASUS_USE_GRAPH_EVENTS

namespace Pegasus {
namespace Graph {
    //! generic class, used by the user to pass event data around
    class IGraphUserData 
    {
    public: 
        IGraphUserData(){}
        virtual ~IGraphUserData(){}
    };

    //! \brief internal dispatching function, for internal use only
    template<class E, class L, class S>
    void Internal_DispatchEvent(S* s, E& e, L* l) 
    {
        if (l != nullptr)
        {
            l->OnEvent(s->GetGraphEventUserData(), e);
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
#define GRAPH_EVENT_BEGIN_REGISTRY(__listener_name) class __listener_name { public: __listener_name(){} virtual ~__listener_name(){}

//! \brief Pass the name of the event struct to register
#define GRAPH_EVENT_REGISTER(__event) virtual void OnEvent(Pegasus::Graph::IGraphUserData * d, __event& e) = 0;

//! \brief Pass the proxies that will get user data injected
#define GRAPH_EVENT_REGISTER_PROXY(__proxy) virtual void OnInitUserData(__proxy* proxy, const char* name) = 0; \
                                            virtual void OnDestroyUserData(__proxy* proxy, const char* name) = 0;

//! \brief call at the end of the event registry once you are done
#define GRAPH_EVENT_END_REGISTRY };

//! \brief declare this statement at the very beginning of your node that will send events. Make sure to pass the event listener type declared in BEGIN_EVENT_REGISTRY
#define GRAPH_EVENT_DECLARE_DISPATCHER(_listener_type) public: \
    Pegasus::Graph::IGraphUserData * GetGraphEventUserData() { return __mUserData;} \
    void SetGraphEventUserData(Pegasus::Graph::IGraphUserData * userData)  { __mUserData = userData; InvalidateData(); } \
    _listener_type * GetEventListener() { return __mEventListener; } \
    void SetEventListener(_listener_type * l) { __mEventListener = l; } \
    private: _listener_type * __mEventListener; Pegasus::Graph::IGraphUserData * __mUserData; public:

//! \brief use this init to inject user data into the proxy
#define GRAPH_EVENT_INIT_USER_DATA(sender, name, listener) Pegasus::Graph::Internal_InitUserData(sender, name, listener)

//! \brief use this destory to destroy user data into the proxy
#define GRAPH_EVENT_DESTROY_USER_DATA(sender, name, listener) Pegasus::Graph::Internal_DestroyUserData(sender, name, listener)

//! \brief init macro, call at the constructors of the Node classes sending events
#define GRAPH_EVENT_INIT_DISPATCHER SetEventListener(nullptr);SetGraphEventUserData(nullptr);

//! \brief call when an event must be dispatched if a listener is attached.
//! \param the sender pointer. "this: pointer usually, or the node graph holding it
//! \param the event type to be dispatched
//! \param the arguments that match the constructor of the event class
#define GRAPH_EVENT_DISPATCH(sender, event, ...) Pegasus::Graph::Internal_DispatchEvent(sender, event(__VA_ARGS__), sender->GetEventListener())

#else
// for events disabled, empty declarations

#define GRAPH_EVENT_BEGIN_REGISTRY(__listener_name) 
#define GRAPH_EVENT_REGISTER_PROXY(__proxy)
#define GRAPH_EVENT_REGISTER(__event) 
#define GRAPH_EVENT_END_REGISTRY
#define GRAPH_EVENT_DECLARE_DISPATCHER(_listener_type)
#define GRAPH_EVENT_INIT_DISPATCHER 
#define GRAPH_EVENT_DISPATCH(sender, event, ...)
#define GRAPH_EVENT_INIT_USER_DATA(sender, name, listener) 
#define GRAPH_EVENT_DESTROY_USER_DATA(sender, name, listener)

#endif //PEGASUS_USE_GRAPH_EVENTS

#endif //PEGASUS_GRAPH_EVENT_DEFS_H

