/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#include "Pegasus/Texture/Texture.h"

namespace Pegasus {
namespace Texture {


Texture::Texture(const TextureConfiguration & configuration)
:   Graph::OutputNode(configuration.GetAllocator()),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

void Texture::SetGeneratorInput(TextureGeneratorIn textureGenerator)
{
    // Check that the configuration is compatible
    if (textureGenerator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(textureGenerator);
    }
    else
    {
        PG_FAILSTR("Unable to set the generator input of a Texture node since their configurations are incompatible");
    }
}

//----------------------------------------------------------------------------------------

void Texture::SetOperatorInput(TextureOperatorIn textureOperator)
{
    // Check that the configuration is compatible
    if (textureOperator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(textureOperator);
    }
    else
    {
        PG_FAILSTR("Unable to set the operator input of a Texture node since their configurations are incompatible");
    }
}

//----------------------------------------------------------------------------------------

TextureDataReturn Texture::GetUpdatedTextureData()
{
    bool updated = false;
    return Graph::OutputNode::GetUpdatedData(updated);
}

//----------------------------------------------------------------------------------------

Texture::~Texture()
{
}


}   // namespace Texture
}   // namespace Pegasus
