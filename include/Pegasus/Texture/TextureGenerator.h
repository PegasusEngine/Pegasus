/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGenerator.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base texture generator node class

#ifndef PEGASUS_TEXTURE_TEXTUREGENERATOR_H
#define PEGASUS_TEXTURE_TEXTUREGENERATOR_H

#include "Pegasus/Graph/GeneratorNode.h"
#include "Pegasus/Texture/TextureConfiguration.h"
#include "Pegasus/Texture/TextureData.h"

namespace Pegasus {
namespace Texture {


//! Base texture generator node class
class TextureGenerator : public Graph::GeneratorNode
{
public:

    //! Constructor
    //! \param configuration Configuration of the generator, such as the resolution and pixel format
    TextureGenerator(const TextureConfiguration & configuration);

    //! Get the configuration of the generator
    //! \return Configuration of the generator, such as the resolution and pixel format
    inline const TextureConfiguration & GetConfiguration() const { return mConfiguration; }


    //! Update the generator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! and returns the dirty flag to the parent caller.
    //! That will trigger a data refresh when calling GetUpdatedData().
    //! \return True if the node data are dirty
    //virtual bool Update() = 0;

    //! Return the texture generator up-to-date data.
    //! \note Defines the standard behavior of all generator nodes.
    //!       Calls GenerateData() if the node data are dirty.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, cannot be a null reference.
    //! \note When asking for the data of a generator node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~TextureGenerator();

    //! Allocate the data associated with the texture generator
    //! \warning Do not override in derived classes since all generators
    //!          use the same texture data class
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const;


    //! Generate the content of the data associated with the texture generator
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(TextureGenerator)

    //! Configuration of the generator, such as the resolution and pixel format
    TextureConfiguration mConfiguration;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREGENERATOR_H
