/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGenerator.cpp
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Base texture generator node class

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(TextureGenerator)
END_IMPLEMENT_PROPERTIES(TextureGenerator)

//----------------------------------------------------------------------------------------

TextureGenerator::TextureGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    BEGIN_INIT_PROPERTIES(TextureGenerator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
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
    BEGIN_INIT_PROPERTIES(TextureGenerator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
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
        PEGASUS_EVENT_DISPATCH (
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
