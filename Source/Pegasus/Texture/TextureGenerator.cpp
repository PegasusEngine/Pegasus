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


TextureGenerator::TextureGenerator(const TextureConfiguration & configuration)
:   Graph::GeneratorNode(configuration.GetAllocator()),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------
    
TextureGenerator::~TextureGenerator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureGenerator::AllocateData() const
{
    //! \todo Use allocator
    return PG_NEW(GetAllocator(), "TextureGenerator::TextureData", Pegasus::Memory::PG_MEM_TEMP) TextureData(mConfiguration);
}


}   // namespace Texture
}   // namespace Pegasus
