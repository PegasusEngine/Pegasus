/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureConfigurationProxy.h
//! \author	Karolyn Boulanger
//! \date	11th June 2014
//! \brief	Proxy interface, used by the editor to interact with a texture configuration

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTURECONFIGURATIONPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTURECONFIGURATIONPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


//! Proxy interface, used by the editor to interact with a texture configuration
class ITextureConfigurationProxy
{
public:

    //! Destructor
    virtual ~ITextureConfigurationProxy() {};


    //! \todo Add support for the texture type

    //! \todo Add support for the pixel format

    //! Get the width of the texture
    //! \return Horizontal resolution of the texture in pixels (>= 1)
    virtual unsigned int GetWidth() const = 0;

    //! Get the height of the texture
    //! \return Vertical resolution of the texture in pixels (>= 1)
    virtual unsigned int GetHeight() const = 0;

    //! Get the depth of the texture
    //! \return Depth of the texture in pixels (>= 1)
    virtual unsigned int GetDepth() const = 0;

    //! Get the number of layers of the texture
    //! \return Number of layers for array textures, 6 for cube maps, 1 otherwise
    virtual unsigned int GetNumLayers() const = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTURECONFIGURATIONPROXY_H
