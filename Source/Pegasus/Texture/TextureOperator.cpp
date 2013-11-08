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
{
}

//----------------------------------------------------------------------------------------

TextureOperator::TextureOperator(const TextureConfiguration & configuration,
                                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
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
        PG_FAILSTR("Cannot set the configuration of a texture operator because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------

TextureOperator::~TextureOperator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * TextureOperator::AllocateData() const
{
    return PG_NEW(GetNodeAllocator(), -1, "TextureOperator::TextureData", Pegasus::Alloc::PG_MEM_TEMP)
                    TextureData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Texture
}   // namespace Pegasus
