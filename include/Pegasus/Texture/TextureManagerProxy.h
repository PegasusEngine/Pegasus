/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManagerProxy.h
//! \author	Kevin Boulanger
//! \date	09th June 2014
//! \brief	Proxy object, used by the editor to interact with the textures

#ifndef PEGASUS_TEXTURE_TEXTUREMANAGERPROXY_H
#define PEGASUS_TEXTURE_TEXTUREMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureManagerProxy.h"
#include "Pegasus/Texture/TextureTracker.h"
#include "Pegasus/Texture/TextureProxy.h"
#include "Pegasus/Texture/TextureManager.h"

namespace Pegasus {
namespace Texture {

class TextureManager;
class ITextureEventListener;


//! Proxy object, used by the editor to interact with the textures
class TextureManagerProxy : public ITextureManagerProxy
{
public:

    // Constructor
    //! \param textureManager Proxied texture manager object, cannot be nullptr
    TextureManagerProxy(TextureManager * textureManager);

    //! Destructor
    virtual ~TextureManagerProxy();

    //! Get the texture manager associated with the proxy
    //! \return Texture manager associated with the proxy (!= nullptr)
    inline TextureManager * GetTextureManager() const { return mTextureManager; }


    //! Get the number of currently allocated textures
    //! \return Number of currently allocated textures
    virtual unsigned int GetNumTextures() const;

    //! Get one of the currently allocated textures
    //! \param index Index of the texture to get (< GetNumTextures())
    //! \return Proxy to the desired texture
    virtual ITextureProxy * GetTexture(unsigned int index) const;

    //! Sets the event listener to be used for the texture manager
    //! \param event listener reference
    virtual void RegisterEventListener(ITextureEventListener * eventListener);

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied texture manager object
    TextureManager * const mTextureManager;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_TEXTUREMANAGERPROXY_H
