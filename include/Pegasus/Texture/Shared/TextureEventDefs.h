/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEventDefs.h
//! \author	Kleber Garcia
//! \date	4th June 2014
//! \brief	Event classes containers for textures

#ifndef PEGASUS_TEXTUREEVENTDEFS_H 
#define PEGASUS_TEXTUREEVENTDEFS_H

#include "Pegasus/Core/Shared/EventDefs.h"

namespace Pegasus {
namespace Texture {

class ITextureNodeProxy;


// Container that fires up when an event has occurred for a texture node (output or any child node)
class TextureNodeNotificationEvent
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
    TextureNodeNotificationEvent(NotificationType type, const char* msg)
        : mMessage(msg), mType(type)
        { }

    //! Destructor
    ~TextureNodeNotificationEvent()
        { }
 
    //! Gets the message contained in this texture notification
    inline const char * GetMessage() const { return mMessage; }
        
    //! Gets the type of notification fired from this event
    inline NotificationType GetType() const { return mType; }

private:
    const char* mMessage;
    NotificationType mType;
};

//----------------------------------------------------------------------------------------

// Event when a generator node has finished operations
class TextureNodeGenerationEvent
{
public:
    enum GenerationEventType
    {
        BEGIN,
        END_SUCCESS,
        END_FAIL
    };
        
    // Constructor
    TextureNodeGenerationEvent(GenerationEventType type)
        : mType(type)
        { }
    
    // Destructor
    ~TextureNodeGenerationEvent()
        { }
        
    //! Gets the type of this generation event
    inline GenerationEventType GetType() const { return mType; }

private:
    GenerationEventType mType;    
};

//----------------------------------------------------------------------------------------

// Event class when an operator node has finished operations
class TextureNodeOperationEvent
{
public:
    enum OperationEventType
    {
        BEGIN,
        END_SUCCESS,
        END_FAIL
    };
        
    // Constructor
    TextureNodeOperationEvent(OperationEventType type)
        : mType(type)
        { }
    
    // Destructor
    ~TextureNodeOperationEvent()
        { }
        
    //! Gets the type of this operation event
    inline OperationEventType GetType() const { return mType; }

private:
    OperationEventType mType;
};

//----------------------------------------------------------------------------------------

// Event declarations
PEGASUS_EVENT_BEGIN_REGISTRY(ITextureNodeEventListener)
    PEGASUS_EVENT_REGISTER(TextureNodeNotificationEvent)
    PEGASUS_EVENT_REGISTER(TextureNodeGenerationEvent)
    PEGASUS_EVENT_REGISTER(TextureNodeOperationEvent)
PEGASUS_EVENT_END_REGISTRY

//! \todo Do we need to declare that proxy?
    //PEGASUS_EVENT_REGISTER_PROXY(ITextureNodeProxy)


}   // namespace Texture
}   // namespace Pegasus


#endif  // PEGASUS_TEXTUREEVENTDEFS_H
