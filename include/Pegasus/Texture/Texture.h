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
        class TextureProxy;
    }
}

namespace Pegasus {
namespace Texture {

// forward declaration
class ITextureFactory;

//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class Texture : public Graph::OutputNode
{
public:

    //! Default constructor, uses the default texture configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    Texture(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \param configuration Configuration of the texture, such as the resolution and pixel format
    Texture(const TextureConfiguration & configuration,
            Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Set the configuration of the texture
    //! \warning Can be done only after the constructor has been called, when no input node is connected yet.
    //!          In case of error, the configuration is not set
    void SetConfiguration(const TextureConfiguration & configuration);

    //! Sets the texture factory, which will generate CPU data
    //! \note Must be set in order to provide GPU data during GetUpdatedData callback
    void SetFactory(ITextureFactory * textureFactory) { mFactory = textureFactory; }

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


    //! Return the texture up-to-date data.
    //! \return Reference to the texture data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note Calls GetUpdatedData() internally, and regenerate the texture data
    //!       if any part of the graph is dirty
    TextureDataReturn GetUpdatedTextureData();

    //! Releases the entire graph data. Propagates to its children recursively
    virtual void ReleaseDataAndPropagate();


    //! Texture node creation function, used by the texture manager
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \return New instance of the texture node
    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
    {   return PG_NEW(nodeAllocator, -1, "Texture", Alloc::PG_MEM_PERM) Texture(nodeAllocator, nodeDataAllocator); }


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the texture
    inline TextureProxy * GetProxy() const { return mProxy; }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

protected:

    //! Destructor
    virtual ~Texture();

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(Texture)

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the texture
    TextureProxy * mProxy;

#endif  // PEGASUS_ENABLE_PROXIES

    void ReleaseGPUData();

    //! Configuration of the texture, such as the resolution and pixel format
    TextureConfiguration mConfiguration;

    //! Pointer to the GPU factory. Generates GPU data from cpu texture data
    ITextureFactory * mFactory;
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
