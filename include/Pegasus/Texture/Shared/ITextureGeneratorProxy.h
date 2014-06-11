/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureGeneratorProxy.h
//! \author	Kevin Boulanger
//! \date	11th June 2014
//! \brief	Proxy interface, used by the editor to interact with a texture generator node

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTUREGENERATORPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTUREGENERATORPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


//! Proxy interface, used by the editor to interact with a texture generator node
class ITextureGeneratorProxy
{
public:

    //! Destructor
    virtual ~ITextureGeneratorProxy() {};


    //! Get the name of the texture generator
    //! \return Name of the texture generator
    virtual const char * GetName() const = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTUREGENERATORPROXY_H
