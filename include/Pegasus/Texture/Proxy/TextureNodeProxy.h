/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureNodeProxy.h
//! \author	Karolyn Boulanger
//! \date	08th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture node

#ifndef PEGASUS_TEXTURE_PROXY_TEXTURENODEPROXY_H
#define PEGASUS_TEXTURE_PROXY_TEXTURENODEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"

namespace Pegasus {
namespace Texture {

class TextureGenerator;
class TextureOperator;
class Texture;


//! Proxy object, used by the editor to interact with a texture node
class TextureNodeProxy : public ITextureNodeProxy
{
public:

    // Constructor with a texture generator node
    //! \param textureGenerator Proxied texture generator node object, cannot be nullptr
    TextureNodeProxy(TextureGenerator* textureGenerator);

    // Constructor with a texture operator node
    //! \param textureOperator Proxied texture operator node object, cannot be nullptr
    TextureNodeProxy(TextureOperator* textureOperator);

    // Constructor with a texture output node
    //! \param texture Proxied texture output node object, cannot be nullptr
    TextureNodeProxy(Texture* texture);

    //! Destructor
    virtual ~TextureNodeProxy();


    //! Get the type of node
    //! \return Type of the proxied node (ITextureNodeProxy::NODETYPE_xxx constant)
    virtual ITextureNodeProxy::NodeType GetNodeType() const override { return mNodeType; }


    //! Get the texture generator node associated with the proxy
    //! \return Texture generator node associated with the proxy
    //! \warning Returns nullptr and an assertion error if mNodeType != NODETYPE_GENERATOR
    TextureGenerator * GetTextureGenerator() const;

    //! Get the texture operator node associated with the proxy
    //! \return Texture operator node associated with the proxy
    //! \warning Returns nullptr and an assertion error if mNodeType != NODETYPE_OPERATOR
    TextureOperator * GetTextureOperator() const;

    //! Get the texture output node associated with the proxy
    //! \return Texture output node associated with the proxy
    //! \warning Returns nullptr and an assertion error if mNodeType != NODETYPE_OUTPUT
    Texture * GetTexture() const;


    //! Get the name of the texture
    //! \return Name of the texture
    virtual const char * GetName() const override;

    //! Get the configuration of the texture
    //! \return Configuration proxy of the texture
    virtual const ITextureConfigurationProxy * GetConfiguration() const override;


    //! Get the number of input nodes connected to the current node
    //! \return Number of input nodes (from 0 to Node::MAX_NUM_INPUTS - 1)
    virtual unsigned int GetNumInputs() const override;

    //! Get one of the input nodes
    //! \param index Index of the input node (< GetNumInputs())
    //! \return Node proxy, nullptr in case of error
    virtual ITextureNodeProxy * GetInputNode(unsigned int index) override;


    //! Get the proxy of the texture's property grid
    //! \return Proxy of the texture's property grid
    virtual PropertyGrid::IPropertyGridObjectProxy * GetPropertyGridObjectProxy() const override;
 
    //! Get the proxy of the decorated object
    virtual IRuntimeAssetObjectProxy* GetDecoratedObject() const;
    

    //! Set the event listener of this texture
    //! \param listener Event listener
    virtual void SetEventListener(ITextureNodeEventListener* listener) override;

    //! Set the user data of this event object
    //! \param userData User data of this texture proxy passed around events
    virtual void SetUserData(Core::IEventUserData* userData) override;

    //! Get the user data of this event object
    //! \return user data of this texture proxy passed around events
    virtual Core::IEventUserData* GetUserData() const override;

    //! If this asset runtime object has a property attached, the return it.
    //! \return the property grid object of this proxy. If it doesn't exist then it returns null.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid();

    //------------------------------------------------------------------------------------

private:

    //! Type of the node (ITextureNodeProxy::NODETYPE_xxx constant)
    const ITextureNodeProxy::NodeType mNodeType;

    //! Proxied texture object
    union
    {
        TextureGenerator* mTextureGeneratorNode;
        TextureOperator* mTextureOperatorNode;
        Texture* mTextureNode;
    };
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTURENODEPROXY_H
