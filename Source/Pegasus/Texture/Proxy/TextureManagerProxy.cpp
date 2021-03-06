/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManagerProxy.cpp
//! \author	Karolyn Boulanger
//! \date	09th June 2014
//! \brief	Proxy object, used by the editor to interact with the textures

//! \todo Why do we need this in Rel-Debug? TextureManagerProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Proxy/TextureManagerProxy.h"
#include "Pegasus/Texture/Proxy/TextureConfigurationProxy.h"
#include "Pegasus/Texture/Proxy/TextureNodeProxy.h"
#include "Pegasus/Texture/TextureManager.h"

namespace Pegasus {
namespace Texture {


TextureManagerProxy::TextureManagerProxy(TextureManager * textureManager)
:   mTextureManager(textureManager)
{
    PG_ASSERTSTR(textureManager != nullptr, "Trying to create a texture manager proxy from an invalid texture manager object");
}

//----------------------------------------------------------------------------------------

TextureManagerProxy::~TextureManagerProxy()
{
}

//----------------------------------------------------------------------------------------

ITextureNodeProxy * TextureManagerProxy::CreateGeneratorNode(const char * className,
                                                             ITextureConfigurationProxy * configurationProxy)
{
    // Convert the abstract configuration proxy to a concrete proxy, relying on the fact that
    // TextureConfigurationProxy is the only possible derived class from ITextureConfigurationProxy
    TextureConfigurationProxy * textureConfigurationProxy = static_cast<TextureConfigurationProxy *>(configurationProxy);
    if (configurationProxy != nullptr)
    {
        TextureGeneratorRef generator = mTextureManager->CreateTextureGeneratorNode(className, *textureConfigurationProxy->GetConfiguration());

        //! \todo What to do with the pointer?

        return generator->GetProxy();
    }
    else
    {
        PG_FAILSTR("Invalid configuration proxy given to the texture generator creation function");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void TextureManagerProxy::RegisterEventListener(ITextureNodeEventListener * eventListener)
{
    mTextureManager->RegisterEventListener(eventListener);
}

}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
