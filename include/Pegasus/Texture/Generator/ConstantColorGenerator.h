/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConstantColorGenerator.h
//! \author	Kevin Boulanger
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
public:

    //! Default constructor
    ConstantColorGenerator();


    //! Update the generator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! and returns the dirty flag to the parent caller.
    //! That will trigger a data refresh when calling GetUpdatedData().
    //! \return True if the node data are dirty
    //virtual bool Update() = 0;

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~ConstantColorGenerator();


    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(ConstantColorGenerator)
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_GENERATOR_CONSTANTCOLORGENERATOR_H
