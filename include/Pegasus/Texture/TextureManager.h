/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManager.h
//! \author	Karolyn Boulanger
//! \date	03rd November 2013
//! \brief	Global texture node manager, including the factory features

#ifndef PEGASUS_TEXTURE_TEXTUREMANAGER_H
#define PEGASUS_TEXTURE_TEXTUREMANAGER_H

#include "Pegasus/Graph/Node.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"
#include "Pegasus/Texture/Proxy/TextureManagerProxy.h"
#include "Pegasus/AssetLib/AssetRuntimeFactory.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Texture/Proxy/TextureTracker.h"
#endif

namespace Pegasus {
    namespace Graph {
        class NodeManager;
    }
}

namespace Pegasus {
namespace Texture {

class ITextureFactory;

#if PEGASUS_USE_EVENTS
class ITextureEventListener;
#endif


//! Global texture node manager, including the factory features
class TextureManager : public AssetLib::AssetRuntimeFactory
{
public:

    //! Constructor
    //! \note Registers all texture nodes of the Texture project
    //! \param nodeManager Pointer to the global node manager (!- nullptr)
    TextureManager(Graph::NodeManager * nodeManager, ITextureFactory * textureFactory);

    //! Destructor
    virtual ~TextureManager();


    //! Register a texture node class, to be called before any node of this type is created
    //! \param className String of the node class (maximum length NodeManager::MAX_CLASS_NAME_LENGTH)
    //! \param createNodeFunc Pointer to the texture node member function that instantiates the node
    //! \warning If the number of registered node classes reaches NodeManager::MAX_NUM_REGISTERED_NODES,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of NodeManager::MAX_NUM_REGISTERED_NODES
    void RegisterTextureNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc);

    //! Create a texture node
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureReturn CreateTextureNode(const TextureConfiguration & configuration);

    //! Create a texture generator node by class name
    //! \param className Name of the texture generator node class to instantiate
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureGeneratorReturn CreateTextureGeneratorNode(const char * className,
                                                      const TextureConfiguration & configuration);

    //! Create an texture operator node by class name
    //! \param className Name of the texture operator node class to instantiate
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureOperatorReturn CreateTextureOperatorNode(const char * className,
                                                    const TextureConfiguration & configuration);

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the texture manager
    //! \return Proxy associated with the texture manager
    //@{
    inline TextureManagerProxy * GetProxy() { return &mProxy; }
    inline const TextureManagerProxy * GetProxy() const { return &mProxy; }
    //@}

    //! Returns the texture tracker, to get a list of texture proxies
    inline const TextureTracker & GetTracker() const { return mTracker; }

#endif  // PEGASUS_ENABLE_PROXIES

#if PEGASUS_USE_EVENTS

    //! Registers an event listener so we can listen to texture specific events whilst constructing nodes.
    //! \param the event listener to use
    void RegisterEventListener(ITextureEventListener * eventListener) { mEventListener = eventListener; }

#endif  // PEGASUS_USE_EVENTS

    //! Creates a runtime object from an asset. This function must add a reference to the 
    //! runtime object returned, (if its ref counted)
    //! \param the asset type requested.
    //! \return the runtime asset created. return null if unsuccessfull.
    virtual AssetLib::RuntimeAssetObjectRef CreateRuntimeObject(const PegasusAssetTypeDesc* desc);

    //! Returns a null terminated list of asset descriptions this runtime factory will accept.
    //! \return a null terminated list of asset descriptions
    virtual const PegasusAssetTypeDesc*const* GetAssetTypes() const;

    //------------------------------------------------------------------------------------
    
private:

    // The texture node manager is unique in each application
    PG_DISABLE_COPY(TextureManager)

        
    //! Register all the texture nodes of the Texture project
    void RegisterAllTextureNodes();


    //! Pointer to the node manager (!= nullptr)
    Graph::NodeManager * mNodeManager;

    //! Pointer to the GPU factory. Generates GPU data from CPU texture data
    ITextureFactory * mFactory;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the texture manager
    TextureManagerProxy mProxy;

    //! Texture tracker, to get a list of texture proxies
    TextureTracker mTracker;

#endif  // PEGASUS_ENABLE_PROXIES

#if PEGASUS_USE_EVENTS
    ITextureEventListener * mEventListener;
#endif
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREMANAGER_H
