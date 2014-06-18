/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureNodeProxy.h
//! \author	Kevin Boulanger
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
    TextureNodeProxy(TextureGenerator * textureGenerator);

    // Constructor with a texture operator node
    //! \param textureOperator Proxied texture operator node object, cannot be nullptr
    TextureNodeProxy(TextureOperator * textureOperator);

    // Constructor with a texture output node
    //! \param texture Proxied texture output node object, cannot be nullptr
    TextureNodeProxy(Texture * texture);

    //! Destructor
    virtual ~TextureNodeProxy();


    //! Get the type of node
    //! \return Type of the proxied node (ITextureNodeProxy::NODETYPE_xxx constant)
    virtual ITextureNodeProxy::NodeType GetNodeType() const { return mNodeType; }


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
    virtual const char * GetName() const;

    //! Get the configuration of the texture
    //! \return Configuration proxy of the texture
    virtual const ITextureConfigurationProxy * GetConfiguration() const;


    //! Get the number of input nodes connected to the current node
    //! \return Number of input nodes (from 0 to Node::MAX_NUM_INPUTS - 1)
    virtual unsigned int GetNumInputs() const;

    //! Get one of the input nodes
    //! \param index Index of the input node (< GetNumInputs())
    //! \return Node proxy, nullptr in case of error
    virtual ITextureNodeProxy * GetInputNode(unsigned int index);

    //------------------------------------------------------------------------------------
    
private:

    //! Type of the node (ITextureNodeProxy::NODETYPE_xxx constant)
    const ITextureNodeProxy::NodeType mNodeType;

    //! Proxied texture object
    union
    {
        TextureGenerator * mTextureGenerator;
        TextureOperator * mTextureOperator;
        Texture * mTexture;
    };
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTURENODEPROXY_H
