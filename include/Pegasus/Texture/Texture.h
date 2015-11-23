/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.h
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#ifndef PEGASUS_TEXTURE_TEXTURE_H
#define PEGASUS_TEXTURE_TEXTURE_H

#include "Pegasus/Graph/OutputNode.h"
#include "Pegasus/Texture/TextureConfiguration.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"
#include "Pegasus/Texture/Proxy/TextureNodeProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

//! forward declarations
namespace Pegasus {
    namespace Texture {
        class ITextureFactory;
        class TextureTracker;
    }

    namespace Graph {
        class NodeManager;
    }
}

namespace Pegasus {
namespace Texture {

//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class Texture : public Graph::OutputNode
{
    BEGIN_DECLARE_PROPERTIES(Texture, OutputNode)
    END_DECLARE_PROPERTIES()

public:

    //! Default constructor, uses the default texture configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    Texture(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \param configuration Configuration of the texture, such as the resolution and pixel format
    Texture(Graph::NodeManager* nodeManager, const TextureConfiguration & configuration,
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
    static Graph::NodeReturn CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
    {   return PG_NEW(nodeAllocator, -1, "Texture", Alloc::PG_MEM_PERM) Texture(nodeManager, nodeAllocator, nodeDataAllocator); }

    //! Get the class name of this object instance.
    virtual const char* GetClassInstanceName() const { return "Texture"; }


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the texture
    //! \return Proxy associated with the texture
    //@{
    virtual AssetLib::IRuntimeAssetObjectProxy * GetProxy() { return &mProxy; }
    virtual const AssetLib::IRuntimeAssetObjectProxy * GetProxy() const { return &mProxy; }
    //@}

    //! Set the tracker associated with the texture
    //! \param tracker Global tracker used to get the list of textures in the editor
    void SetTracker(TextureTracker * tracker);

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
    TextureNodeProxy mProxy;

    //! Tracker associated with the texture
    TextureTracker * mTracker;
#endif  // PEGASUS_ENABLE_PROXIES

    void ReleaseGPUData();

    //! Configuration of the texture, such as the resolution and pixel format
    TextureConfiguration mConfiguration;

    //! Pointer to the GPU factory. Generates GPU data from CPU texture data
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
