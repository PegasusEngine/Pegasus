/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConstantColorGenerator.h
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture generator that fills the image with a constant color

#ifndef PEGASUS_TEXTURE_GENERATOR_CONSTANTCOLORGENERATOR_H
#define PEGASUS_TEXTURE_GENERATOR_CONSTANTCOLORGENERATOR_H

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


//! Texture generator that fills the image with a constant color
class ConstantColorGenerator : public TextureGenerator
{
    DECLARE_TEXTURE_GENERATOR_NODE(ConstantColorGenerator)

    BEGIN_DECLARE_PROPERTIES(ConstantColorGenerator, TextureGenerator)
        DECLARE_PROPERTY(Math::Color8RGBA, Color, Math::Color8RGBA(0, 0, 0, 255))
    END_DECLARE_PROPERTIES()

    //------------------------------------------------------------------------------------
    
public:

    //! Update the generator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! and returns the dirty flag to the parent caller.
    //! That will trigger a data refresh when calling GetUpdatedData().
    //! \return True if the node data is dirty
    //virtual bool Update();

    //------------------------------------------------------------------------------------
    
protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_GENERATOR_CONSTANTCOLORGENERATOR_H
