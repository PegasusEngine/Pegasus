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


TextureOperator::TextureOperator()
:    Graph::OperatorNode()
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
    return PG_CORE_NEW("TextureOperator::TextureData", Pegasus::Memory::PG_MEM_TEMP) TextureData();
}


}   // namespace Texture
}   // namespace Pegasus
