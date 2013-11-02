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

namespace Pegasus {
namespace Texture {


//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class Texture : public Graph::OutputNode
{
public:

    //! Default constructor
    Texture();

    //! Append a node to the list of input nodes
    //! \param inputNode Texture node to add to the list of input nodes (equivalent to NodeIn)
    //! \warning Fails if an input node is already added, as this node supports one input only
    //! \warning This function must be called once since an output requires one input
    //virtual void AddInput(TextureIn inputNode);


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
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTURE_H
