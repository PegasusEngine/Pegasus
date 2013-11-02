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
:   Graph::GeneratorNode(),
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
    return PG_CORE_NEW("TextureGenerator::TextureData", Pegasus::Memory::PG_MEM_TEMP) TextureData();
}


}   // namespace Texture
}   // namespace Pegasus
