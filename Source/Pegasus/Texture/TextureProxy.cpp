/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureProxy.cpp
//! \author	Kevin Boulanger
//! \date	08th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture

//! \todo Why do we need this in Rel-Debug? TextureProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/TextureProxy.h"
#include "Pegasus/Texture/Texture.h"

namespace Pegasus {
namespace Texture {


TextureProxy::TextureProxy(Texture * texture)
:   mTexture(texture)
{
    PG_ASSERTSTR(texture != nullptr, "Trying to create a texture proxy from an invalid texture object");
}

//----------------------------------------------------------------------------------------

TextureProxy::~TextureProxy()
{
}

//----------------------------------------------------------------------------------------

const char * TextureProxy::GetName() const
{
    return mTexture->GetName();
}

//----------------------------------------------------------------------------------------

unsigned int TextureProxy::GetWidth() const
{
    return mTexture->GetConfiguration().GetWidth();
}

//----------------------------------------------------------------------------------------

unsigned int TextureProxy::GetHeight() const
{
    return mTexture->GetConfiguration().GetHeight();
}

//----------------------------------------------------------------------------------------

unsigned int TextureProxy::GetDepth() const
{
    return mTexture->GetConfiguration().GetDepth();
}

//----------------------------------------------------------------------------------------

unsigned int TextureProxy::GetNumLayers() const
{
    return mTexture->GetConfiguration().GetNumLayers();
}


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
