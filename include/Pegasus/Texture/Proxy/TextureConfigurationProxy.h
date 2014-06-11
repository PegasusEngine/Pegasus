/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureConfigurationProxy.h
//! \author	Kevin Boulanger
//! \date	11th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture configuration

#ifndef PEGASUS_TEXTURE_PROXY_TEXTURECONFIGURATIONPROXY_H
#define PEGASUS_TEXTURE_PROXY_TEXTURECONFIGURATIONPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureConfigurationProxy.h"

namespace Pegasus {
namespace Texture {

class TextureConfiguration;


//! Proxy object, used by the editor to interact with a texture configuration
class TextureConfigurationProxy : public ITextureConfigurationProxy
{
public:

    // Constructor
    //! \param configuration Proxied texture configuration object, cannot be nullptr
    TextureConfigurationProxy(TextureConfiguration * configuration);

    //! Destructor
    virtual ~TextureConfigurationProxy();

    //! Get the texture configuration associated with the proxy
    //! \return Texture configuration associated with the proxy (!= nullptr)
    inline TextureConfiguration * GetConfiguration() const { return mConfiguration; }


    //! \todo Add support for the texture type

    //! \todo Add support for the pixel format

    //! Get the width of the texture
    //! \return Horizontal resolution of the texture in pixels (>= 1)
    virtual unsigned int GetWidth() const;

    //! Get the height of the texture
    //! \return Vertical resolution of the texture in pixels (>= 1)
    virtual unsigned int GetHeight() const;

    //! Get the depth of the texture
    //! \return Depth of the texture in pixels (>= 1)
    virtual unsigned int GetDepth() const;

    //! Get the number of layers of the texture
    //! \return Number of layers for array textures, 6 for cube maps, 1 otherwise
    virtual unsigned int GetNumLayers() const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied texture configuration object
    TextureConfiguration * const mConfiguration;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTURECONFIGURATIONPROXY_H
