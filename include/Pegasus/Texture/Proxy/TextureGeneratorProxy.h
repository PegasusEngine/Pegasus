/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGeneratorProxy.h
//! \author	Kevin Boulanger
//! \date	11th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture generator node

#ifndef PEGASUS_TEXTURE_PROXY_TEXTUREGENERATORPROXY_H
#define PEGASUS_TEXTURE_PROXY_TEXTUREGENERATORPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Shared/ITextureGeneratorProxy.h"

namespace Pegasus {
namespace Texture {

class TextureGenerator;


//! Proxy object, used by the editor to interact with a texture generator node
class TextureGeneratorProxy : public ITextureGeneratorProxy
{
public:

    // Constructor
    //! \param generator Proxied texture generator object, cannot be nullptr
    TextureGeneratorProxy(TextureGenerator * generator);

    //! Destructor
    virtual ~TextureGeneratorProxy();

    //! Get the texture generator associated with the proxy
    //! \return Texture generator associated with the proxy (!= nullptr)
    inline TextureGenerator * GetGenerator() const { return mGenerator; }


    //! Get the name of the texture generator
    //! \return Name of the texture generator
    virtual const char * GetName() const;

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied texture generator object
    TextureGenerator * const mGenerator;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_PROXY_TEXTUREGENERATORPROXY_H
