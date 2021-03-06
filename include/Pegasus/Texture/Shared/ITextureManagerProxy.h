/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureManagerProxy.h
//! \author	Karolyn Boulanger
//! \date	09th June 2014
//! \brief	Proxy interface, used by the editor to interact with the textures

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {

class ITextureNodeProxy;
class ITextureConfigurationProxy;
class ITextureNodeEventListener;


//! Proxy interface, used by the editor to interact with the textures
class ITextureManagerProxy
{
public:

    //! Destructor
    virtual ~ITextureManagerProxy() {};

    //! Create a texture generator node by class name
    //! \param className Name of the texture generator node class to instantiate
    //! \param configurationProxy Proxy for the configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    virtual ITextureNodeProxy * CreateGeneratorNode(const char * className,
                                                    ITextureConfigurationProxy * configurationProxy) = 0;


    //! Sets the event listener to be used for the texture manager
    //! \param event listener reference
    virtual void RegisterEventListener(ITextureNodeEventListener * eventListener) = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTUREMANAGERPROXY_H
