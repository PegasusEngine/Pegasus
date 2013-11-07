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


TextureGenerator::TextureGenerator(Memory::IAllocator * nodeAllocator, Memory::IAllocator * nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
}

//----------------------------------------------------------------------------------------

TextureGenerator::TextureGenerator(const TextureConfiguration & configuration,
                                   Memory::IAllocator * nodeAllocator, Memory::IAllocator * nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
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
        PG_FAILSTR("Cannot set the configuration of a texture generator because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------
    
TextureGenerator::~TextureGenerator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureGenerator::AllocateData() const
{
    return PG_NEW(GetNodeAllocator(), "TextureGenerator::TextureData", Pegasus::Memory::PG_MEM_TEMP)
                    TextureData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Texture
}   // namespace Pegasus
