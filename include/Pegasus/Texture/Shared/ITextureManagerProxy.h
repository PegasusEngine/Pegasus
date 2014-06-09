/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureManagerProxy.h
//! \author	Kevin Boulanger
//! \date	09th June 2014
//! \brief	Proxy interface, used by the editor to interact with the textures

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {

class ITextureProxy;


//! Proxy interface, used by the editor to interact with the textures
class ITextureManagerProxy
{
public:

    //! Destructor
    virtual ~ITextureManagerProxy() {};


    //! Get the number of currently allocated textures
    //! \return Number of currently allocated textures
    virtual unsigned int GetNumTextures() const = 0;

    //! Get one of the currently allocated textures
    //! \param index Index of the texture to get (< GetNumTextures())
    //! \return Proxy to the desired texture
    virtual ITextureProxy * GetTexture(unsigned int index) const = 0;


    //! \todo Implement event listeners
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H
