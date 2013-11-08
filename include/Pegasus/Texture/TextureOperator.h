/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureOperator.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base texture operator node class

#ifndef PEGASUS_TEXTURE_TEXTUREOPERATOR_H
#define PEGASUS_TEXTURE_TEXTUREOPERATOR_H

#include "Pegasus/Graph/OperatorNode.h"
#include "Pegasus/Texture/TextureConfiguration.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Texture/TextureDeclaration.h"

namespace Pegasus {
namespace Texture {


//! Base texture operator node class
//! \warning IMPORTANT! When deriving from this class, update TextureManager::RegisterAllTextureNodes()
//!                     so the operator node can be instantiated
class TextureOperator : public Graph::OperatorNode
{
public:

    //! Default constructor, uses the default texture configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    TextureOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \param configuration Configuration of the operator, such as the resolution and pixel format
    TextureOperator(const TextureConfiguration & configuration,
                    Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Set the configuration of the texture operator
    //! \warning Can be done only after the constructor has been called
    void SetConfiguration(const TextureConfiguration & configuration);

    //! Get the configuration of the operator
    //! \return Configuration of the operator, such as the resolution and pixel format
    inline const TextureConfiguration & GetConfiguration() const { return mConfiguration; }


    //! Return the texture operator up-to-date data.
    //! \note Defines the standard behavior of all texture operator nodes.
    //!       Calls GetUpdatedData() on all inputs, and if any of them was dirty,
    //!       then calls GenerateData() to update the node data.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note When asking for the data of a node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~TextureOperator();

    //! Allocate the data associated with the texture operator
    //! \warning Do not override in derived classes since all operators
    //!          use the same texture data class
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const;


    //! Generate the content of the data associated with the texture operator
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(TextureOperator)

    //! Configuration of the operator, such as the resolution and pixel format
    TextureConfiguration mConfiguration;
};

//----------------------------------------------------------------------------------------

//! Reference to a TextureOperator, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TextureOperator>   TextureOperatorRef;

//! Const reference to a reference to a TextureOperator, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TextureOperator> & TextureOperatorIn;

//! Reference to a reference to a TextureOperator, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TextureOperator> & TextureOperatorInOut;

//! Reference to a TextureOperator, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TextureOperator>   TextureOperatorReturn;


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREOPERATOR_H
