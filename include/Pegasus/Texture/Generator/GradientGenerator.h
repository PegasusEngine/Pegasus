/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GradientGenerator.h
//! \author	Kevin Boulanger
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

    BEGIN_DECLARE_PROPERTIES()
        DECLARE_PROPERTY(Math::Color8RGBA, Color0)
        DECLARE_PROPERTY(Math::Color8RGBA, Color1)
        DECLARE_PROPERTY(Math::Vec3, Point0)
        DECLARE_PROPERTY(Math::Vec3, Point1)
    END_DECLARE_PROPERTIES()

public:

    //! Update the generator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! and returns the dirty flag to the parent caller.
    //! That will trigger a data refresh when calling GetUpdatedData().
    //! \return True if the node data are dirty
    //virtual bool Update();

    //------------------------------------------------------------------------------------
    
protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_GENERATOR_GRADIENTGENERATOR_H
