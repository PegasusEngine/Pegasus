/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureProxy.h
//! \author	Kevin Boulanger
//! \date	08th June 2014
//! \brief	Proxy interface, used by the editor to interact with the textures

#ifndef PEGASUS_TIMELINE_SHARED_ITEXTUREPROXY_H
#define PEGASUS_TIMELINE_SHARED_ITEXTUREPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


//! Proxy interface, used by the editor to interact with the textures
class ITextureProxy
{
public:

    //! Destructor
    virtual ~ITextureProxy() {};


    //! Get the name of the texture
    //! \return Name of the texture
    virtual const char * GetName() const = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_SHARED_ITEXTUREPROXY_H
