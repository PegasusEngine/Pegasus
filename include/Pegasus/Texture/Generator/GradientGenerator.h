/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GradientGenerator.h
//! \author	Karolyn Boulanger
//! \date	11th May 2014
//! \brief	Texture generator that renders a gradient

#ifndef PEGASUS_TEXTURE_GENERATOR_GRADIENTGENERATOR_H
#define PEGASUS_TEXTURE_GENERATOR_GRADIENTGENERATOR_H

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


//! Texture generator that renders a gradient
class GradientGenerator : public TextureGenerator
{
    DECLARE_TEXTURE_GENERATOR_NODE(GradientGenerator)

    BEGIN_DECLARE_PROPERTIES(GradientGenerator, TextureGenerator)
        DECLARE_PROPERTY(Math::Color8RGBA, Color0, Math::Color8RGBA(0, 0, 0, 255))
        DECLARE_PROPERTY(Math::Color8RGBA, Color1, Math::Color8RGBA(255, 255, 255, 255))
        DECLARE_PROPERTY(Math::Vec3, Point0, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, Point1, Math::Vec3(1.0f, 0.0f, 0.0f))
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

#endif  // PEGASUS_TEXTURE_GENERATOR_GRADIENTGENERATOR_H
