/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#ifndef PEGASUS_TEXTURE_TEXTURE_H
#define PEGASUS_TEXTURE_TEXTURE_H

#include "Pegasus/Graph/OutputNode.h"
#include "Pegasus/Texture/TextureConfiguration.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"

namespace Pegasus {
namespace Texture {


//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class Texture : public Graph::OutputNode
{
public:

    //! Constructor
    //! \param configuration Configuration of the texture, such as the resolution and pixel format
    Texture(const TextureConfiguration & configuration);

    //! Get the configuration of the texture
    //! \return Configuration of the texture, such as the resolution and pixel format
    inline const TextureConfiguration & GetConfiguration() const { return mConfiguration; }


    //! Set the input of the texture output node to a generator node
    //! \warning If an input was already set (generator or operator), it is replaced
    //! \note If the generator is incompatible in resolution or pixel format with the current node,
    //!       an assertion error is thrown, and nothing is replaced
    //! \param textureGenerator Generator node to use as input
    void SetGeneratorInput(TextureGeneratorIn textureGenerator);

    //! Set the input of the texture output node to a operator node
    //! \warning If an input was already set (generator or operator), it is replaced
    //! \note If the operator is incompatible in resolution or pixel format with the current node,
    //!       an assertion error is thrown, and nothing is replaced
    //! \param textureOperator Operator node to use as input
    void SetOperatorInput(TextureOperatorIn textureOperator);


    //! Update the texture internal state by pulling external parameters.
    //! \note Does only call Update() for the input node.
    //!       That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data of the input node are dirty or if any input node is.
    //virtual bool Update() = 0;

    //! Return the texture up-to-date data.
    //! \return Reference to the texture data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note Calls GetUpdatedData() internally, and regenerate the texture data
    //!       if any part of the graph is dirty
    TextureDataReturn GetUpdatedTextureData();

    //------------------------------------------------------------------------------------

protected:

    //! Destructor
    virtual ~Texture();

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(Texture)

    //! Configuration of the texture, such as the resolution and pixel format
    TextureConfiguration mConfiguration;
};

//----------------------------------------------------------------------------------------

//! Reference to a Texture, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<Texture>   TextureRef;

//! Const reference to a reference to a Texture, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<Texture> & TextureIn;

//! Reference to a reference to a Texture, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<Texture> & TextureInOut;

//! Reference to a Texture, typically used as the return value of a function
typedef       Pegasus::Core::Ref<Texture>   TextureReturn;


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTURE_H
