/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureProxy.h
//! \author	Kevin Boulanger
//! \date	08th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture

#ifndef PEGASUS_TEXTURE_PROXY_TEXTUREPROXY_H
#define PEGASUS_TEXTURE_PROXY_TEXTUREPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureProxy.h"

namespace Pegasus {
namespace Texture {

class Texture;


//! Proxy object, used by the editor to interact with a texture
class TextureProxy : public ITextureProxy
{
public:

    // Constructor
    //! \param texture Proxied texture object, cannot be nullptr
    TextureProxy(Texture * texture);

    //! Destructor
    virtual ~TextureProxy();

    //! Get the texture associated with the proxy
    //! \return Texture associated with the proxy (!= nullptr)
    inline Texture * GetTexture() const { return mTexture; }


    //! Get the name of the texture
    //! \return Name of the texture
    virtual const char * GetName() const;

    //! Get the configuration of the texture
    //! \return Configuration proxy of the texture
    virtual const ITextureConfigurationProxy * GetConfiguration() const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied texture object
    Texture * const mTexture;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTUREPROXY_H
