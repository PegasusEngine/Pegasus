/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureOperator.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base texture operator node class

#include "Pegasus/Texture/TextureOperator.h"

namespace Pegasus {
namespace Texture {


TextureOperator::TextureOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
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
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
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
        GRAPH_EVENT_DISPATCH (
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
        GRAPH_EVENT_DISPATCH (
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
        GRAPH_EVENT_DISPATCH (
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
        GRAPH_EVENT_DISPATCH (
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
        GRAPH_EVENT_DISPATCH (
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
