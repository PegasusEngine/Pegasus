/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureProxy.h
//! \author	Kevin Boulanger
//! \date	08th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture

#ifndef PEGASUS_TEXTURE_TEXTUREPROXY_H
#define PEGASUS_TEXTURE_TEXTUREPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureProxy.h"

namespace Pegasus {
    namespace Texture {
        class Texture;
    }
}

namespace Pegasus {
namespace Texture {


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

    //! Proxied texture object
    Texture * const mTexture;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_TEXTUREPROXY_H
