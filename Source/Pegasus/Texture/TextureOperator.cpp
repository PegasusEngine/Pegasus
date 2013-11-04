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


TextureOperator::TextureOperator(const TextureConfiguration & configuration)
:   Graph::OperatorNode(configuration.GetAllocator()),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

TextureOperator::~TextureOperator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureOperator::AllocateData() const
{
    //! \todo Use allocator
    return PG_NEW(GetAllocator(), "TextureOperator::TextureData", Pegasus::Memory::PG_MEM_TEMP) TextureData(mConfiguration);
}


}   // namespace Texture
}   // namespace Pegasus
