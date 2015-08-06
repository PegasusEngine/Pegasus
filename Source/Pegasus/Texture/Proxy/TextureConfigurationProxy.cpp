/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureConfigurationProxy.cpp
//! \author	Karolyn Boulanger
//! \date	11th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture configuration

//! \todo Why do we need this in Rel-Debug? TextureConfigurationProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Proxy/TextureConfigurationProxy.h"
#include "Pegasus/Texture/TextureConfiguration.h"

namespace Pegasus {
namespace Texture {


TextureConfigurationProxy::TextureConfigurationProxy(TextureConfiguration * configuration)
:   mConfiguration(configuration)
{
    PG_ASSERTSTR(configuration != nullptr, "Trying to create a texture configuration proxy from an invalid texture configuration object");
}

//----------------------------------------------------------------------------------------

TextureConfigurationProxy::~TextureConfigurationProxy()
{
}

//----------------------------------------------------------------------------------------

unsigned int TextureConfigurationProxy::GetWidth() const
{
    return mConfiguration->GetWidth();
}

//----------------------------------------------------------------------------------------

unsigned int TextureConfigurationProxy::GetHeight() const
{
    return mConfiguration->GetHeight();
}

//----------------------------------------------------------------------------------------

unsigned int TextureConfigurationProxy::GetDepth() const
{
    return mConfiguration->GetDepth();
}

//----------------------------------------------------------------------------------------

unsigned int TextureConfigurationProxy::GetNumLayers() const
{
    return mConfiguration->GetNumLayers();
}


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
