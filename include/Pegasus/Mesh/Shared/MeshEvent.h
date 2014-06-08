/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshEvent.h
//! \author	Kleber Garcia
//! \date	8th June 2014
//! \brief	Event classes containers for meshes

#ifndef PEGASUS_MESHEVENT_H
#define PEGASUS_MESHEVENT_H

#include "Pegasus/Graph/Shared/GraphEventDefs.h"

namespace Pegasus
{

namespace Mesh
{

    // event class container that fires up when an event has occured
    class MeshNotificationEvent
    {
    public:
        enum NotificationType
        {
            WARNING
        };

        //! Constructor
        MeshNotificationEvent (NotificationType type, const char * msg)
        : mMessage(msg), mType(type)
        {
        }

        //! Destructor
        ~MeshNotificationEvent(){}

        //! Gets the message contained in this event notification
        const char * GetMessage() const { return mMessage; }
            
        //! Gets the notification type for this event
        NotificationType GetType() const { return mType; }
    private:
        const char * mMessage;
        NotificationType mType;
    };

    // event class when a generator node has finished operations
    class MeshGenerationEvent
    {
    public:
        enum GenerationEventType
        {
            BEGIN,
            END_SUCCESS,
            END_FAIL
        };


        // Constructor
        MeshGenerationEvent (GenerationEventType type)
        : mType(type)
        {
        }

        // Destructor
        ~MeshGenerationEvent() {}

        //! Gets the type of this generation event
        GenerationEventType GetType() const { return mType; }

    private:
        GenerationEventType mType;
    };

    // event class when a operator node has finished operations
    class MeshOperationEvent
    {
    public:
        enum OperationEventType
        {
            BEGIN,
            END_SUCCESS,
            END_FAIL
        };

        // Constructor
        MeshOperationEvent (OperationEventType type)
        : mType(type)
        {
        }

        ~MeshOperationEvent() {}

        //! Gets the type of this operation event
        OperationEventType GetType() const { return mType; }

    private:
        OperationEventType mType;
    };

    GRAPH_EVENT_BEGIN_REGISTRY (IMeshEventListener)
        GRAPH_EVENT_REGISTER(MeshNotificationEvent)
        GRAPH_EVENT_REGISTER(MeshGenerationEvent)
        GRAPH_EVENT_REGISTER(MeshOperationEvent)
    GRAPH_EVENT_END_REGISTRY

} //namespace Mesh
} //namespace Pegasus

#endif
