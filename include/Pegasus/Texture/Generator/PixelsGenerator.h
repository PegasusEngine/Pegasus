/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PixelsGenerator.h
//! \author	Kevin Boulanger
//! \date	17th June 2013
//! \brief	Texture generator that renders randomly located pixels

#ifndef PEGASUS_TEXTURE_GENERATOR_PIXELSGENERATOR_H
#define PEGASUS_TEXTURE_GENERATOR_PIXELSGENERATOR_H

#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


//! Texture generator that renders randomly located pixels
class PixelsGenerator : public TextureGenerator
{
    DECLARE_TEXTURE_GENERATOR_NODE(PixelsGenerator)

    BEGIN_DECLARE_PROPERTIES()
        DECLARE_PROPERTY(unsigned int, NumPixels)
        DECLARE_PROPERTY(unsigned int, Seed)
        DECLARE_PROPERTY(Math::Color8RGBA, BackgroundColor)
        DECLARE_PROPERTY(Math::Color8RGBA, Color0)
    END_DECLARE_PROPERTIES()

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

#endif  // PEGASUS_TEXTURE_GENERATOR_PIXELSGENERATOR_H
