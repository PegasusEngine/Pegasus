/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureData.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture node data, used by all texture nodes, including generators and operators

#ifndef PEGASUS_TEXTURE_TEXTUREDATA_H
#define PEGASUS_TEXTURE_TEXTUREDATA_H

#include "Pegasus/Graph/NodeData.h"

namespace Pegasus {
namespace Texture {


//! Texture node data, used by all texture nodes, including generators and operators
class TextureData : public Graph::NodeData
{
    //friend class Pegasus::Core::Ref<NodeData>;
    //friend class Node;

public:

    //! Default constructor
    //! \note Sets the dirty flag
    TextureData();

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~TextureData();

    //------------------------------------------------------------------------------------
    
private:

    // Node data cannot be copied, only references to them
    PG_DISABLE_COPY(TextureData)

    //! \todo Add actual data members
};

//----------------------------------------------------------------------------------------

////! Reference to a NodeData, typically used when declaring a variable of reference type
//typedef       Pegasus::Core::Ref<NodeData>   NodeDataRef;
//
////! Const reference to a reference to a NodeData, typically used as input parameter of a function
//typedef const Pegasus::Core::Ref<NodeData>   NodeDataIn;
//
////! Reference to a reference to a NodeData, typically used as output parameter of a function
//typedef       Pegasus::Core::Ref<NodeData> & NodeDataInOut;
//
////! Reference to a NodeData, typically used as the return value of a function
//typedef       Pegasus::Core::Ref<NodeData>   NodeDataReturn;


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREDATA_H
