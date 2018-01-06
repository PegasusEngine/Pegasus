/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TexCustomGenerator.h
//! \author	Kleber Garcia
//! \date	January 9th 2018
//! \brief	Custom generator for texture

#ifndef PEGASUS_MESH_CUSTOM_GEN_H
#define PEGASUS_MESH_CUSTOM_GEN_H

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


//! Texture generator that fills the image with a constant color
class TexCustomGenerator : public TextureGenerator
{
    DECLARE_TEXTURE_GENERATOR_NODE(TexCustomGenerator)

public:

    TextureDataRef EditTextureData();
    
protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_GENERATOR_CONSTANTCOLORGENERATOR_H
