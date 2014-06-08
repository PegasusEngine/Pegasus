/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEvent.h
//! \author	Kleber Garcia
//! \date	4rth June 2014
//! \brief	Event classes containers for textures

#ifndef PEGASUS_TEXTUREEVENT_H 
#define PEGASUS_TEXTUREEVENT_H

#include "Pegasus/Graph/Shared/GraphEventDefs.h"

namespace Pegasus
{

namespace Texture
{

    // event class container that fires up when an event has occured
    class TextureNotificationEvent
    {
    public:
        enum NotificationType
        {
            WARNING,
            CONFIGURATION_ERROR,
            INCOMPATIBILITY_WARNING,
            GENERATION_ERROR
        };

        //! Constructor
        TextureNotificationEvent (NotificationType type, const char * msg)
        : mMessage(msg), mType(type)
        {
        }
        
        //! Destructor
        ~TextureNotificationEvent(){}
 
        //! Gets the message contained in this texture notification
        const char * GetMessage() const { return mMessage; }
        
        //! Gets the type of notification fired from this event
        NotificationType GetType() const { return mType; }

    private:
        const char * mMessage;
        NotificationType mType;
    };

    // event class when a generator node has finished operations
    class TextureGenerationEvent
    {
    public:
        enum GenerationEventType
        {
            BEGIN,
            END_SUCCESS,
            END_FAIL
        };
        
        // Constructor
        TextureGenerationEvent(GenerationEventType type)
        : mType(type)
        {
        }
    
        // Destructor
        ~TextureGenerationEvent() {}
        
        //! Gets the type of this generation event
        GenerationEventType GetType() const { return mType; }

    private:
        GenerationEventType mType;
        
    };

    // event class when a operator node has finished operations
    class TextureOperationEvent
    {
    public:
        enum OperationEventType
        {
            BEGIN,
            END_SUCCESS,
            END_FAIL
        };
        
        // Constructor
        TextureOperationEvent(OperationEventType type)
        : mType(type)
        {
        }
    
        // Destructor
        ~TextureOperationEvent() {}
        
        //! Gets the type of this operation event
        OperationEventType GetType() const { return mType; }

    private:
        OperationEventType mType;
        
    };
    GRAPH_EVENT_BEGIN_REGISTRY (ITextureEventListener)
        GRAPH_EVENT_REGISTER (TextureNotificationEvent)
        GRAPH_EVENT_REGISTER (TextureGenerationEvent)
        GRAPH_EVENT_REGISTER (TextureOperationEvent)
    GRAPH_EVENT_END_REGISTRY
}

}


#endif
