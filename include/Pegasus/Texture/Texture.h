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
    //! \param generator Generator node to use as input
    //void SetGeneratorInput(TextureGeneratorIn generator);

    //! Set the input of the texture output node to a operator node
    //! \warning If an input was already set (generator or operator), it is replaced
    //! \note If the operator is incompatible in resolution or pixel format with the current node,
    //!       an assertion error is thrown, and nothing is replaced
    //! \param operator Operator node to use as input
    //void SetOperatorInput(TextureOperatorIn generator);


    //! Update the texture internal state by pulling external parameters.
    //! \note Does only call Update() for the input node.
    //!       That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data of the input node are dirty or if any input node is.
    //virtual bool Update() = 0;

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


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTURE_H
