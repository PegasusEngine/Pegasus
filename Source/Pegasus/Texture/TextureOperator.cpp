/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureOperator.cpp
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Base texture operator node class

#include "Pegasus/Texture/TextureOperator.h"

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(TextureOperator)
END_IMPLEMENT_PROPERTIES(TextureOperator)

//----------------------------------------------------------------------------------------

TextureOperator::TextureOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    BEGIN_INIT_PROPERTIES(TextureOperator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

TextureOperator::TextureOperator(const TextureConfiguration & configuration,
                                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    BEGIN_INIT_PROPERTIES(TextureOperator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

void TextureOperator::SetConfiguration(const TextureConfiguration & configuration)
{
    if (GetRefCount() == 1)
    {
        mConfiguration = configuration;
    }
    else
    {
        PEGASUS_EVENT_DISPATCH (
            this,
            TextureNotificationEvent,
            //event specific arguments
            TextureNotificationEvent::CONFIGURATION_ERROR,
            "Cannot set the configuration of a texture operator because the node is already in use"
        );
    }
}

//----------------------------------------------------------------------------------------

void TextureOperator::AddGeneratorInput(TextureGeneratorIn textureGenerator)
{
    // Check that the configuration is compatible
    if (textureGenerator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        Graph::OperatorNode::AddInput(textureGenerator);
    }
    else
    {
        PEGASUS_EVENT_DISPATCH (
            this,
            TextureNotificationEvent,
            //event specific arguments
            TextureNotificationEvent::INCOMPATIBILITY_WARNING,
            "Unable to add a generator input to a TextureOperator node since their configurations are incompatible"
        );
    }
}

//----------------------------------------------------------------------------------------

void TextureOperator::AddOperatorInput(const Pegasus::Core::Ref<TextureOperator> & textureOperator)
{
    // Check that the configuration is compatible
    if (textureOperator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        Graph::OperatorNode::AddInput(textureOperator);
    }
    else
    {
        PEGASUS_EVENT_DISPATCH (
            this,
            TextureNotificationEvent,
            //event specific arguments
            TextureNotificationEvent::INCOMPATIBILITY_WARNING,
            "Unable to add an operator input to a TextureOperator node since their configurations are incompatible"
        );
    }
}

//----------------------------------------------------------------------------------------

void TextureOperator::ReplaceInputByGenerator(unsigned int index, TextureGeneratorIn textureGenerator)
{
    // Check that the configuration is compatible
    if (textureGenerator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        Graph::OperatorNode::ReplaceInput(index, textureGenerator);
    }
    else
    {
        PEGASUS_EVENT_DISPATCH (
            this,
            TextureNotificationEvent,
            //event specific arguments
            TextureNotificationEvent::INCOMPATIBILITY_WARNING,
            "Unable to replace an input of a TextureOperator node since the new configuration is incompatible with the current one"
        );
    }
}

//----------------------------------------------------------------------------------------

void TextureOperator::ReplaceInputByOperator(unsigned int index, const Pegasus::Core::Ref<TextureOperator> & textureOperator)
{
    // Check that the configuration is compatible
    if (textureOperator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        Graph::OperatorNode::ReplaceInput(index, textureOperator);
    }
    else
    {
        PEGASUS_EVENT_DISPATCH (
            this,
            TextureNotificationEvent,
            //event specific arguments
            TextureNotificationEvent::INCOMPATIBILITY_WARNING,
            "Unable to replace an input of a TextureOperator node since the new configuration is incompatible with the current one"
        );
    }
}

//----------------------------------------------------------------------------------------

TextureOperator::~TextureOperator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureOperator::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "TextureOperator::TextureData", Pegasus::Alloc::PG_MEM_TEMP)
                  TextureData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Texture
}   // namespace Pegasus
