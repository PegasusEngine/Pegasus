/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGenerator.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base texture generator node class

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


TextureGenerator::TextureGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

TextureGenerator::TextureGenerator(const TextureConfiguration & configuration,
                                   Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

void TextureGenerator::SetConfiguration(const TextureConfiguration & configuration)
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
            "Cannot set the configuration of a texture generator because the node is already in use"
        );
    }
}

//----------------------------------------------------------------------------------------
    
TextureGenerator::~TextureGenerator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureGenerator::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "TextureGenerator::TextureData", Pegasus::Alloc::PG_MEM_TEMP)
                    TextureData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Texture
}   // namespace Pegasus
