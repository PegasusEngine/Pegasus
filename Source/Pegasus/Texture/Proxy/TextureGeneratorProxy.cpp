/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGeneratorProxy.cpp
//! \author	Kevin Boulanger
//! \date	11th June 2014
//! \brief	Proxy object, used by the editor to interact with a texture generator node

//! \todo Why do we need this in Rel-Debug? TextureGeneratorProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/Proxy/TextureGeneratorProxy.h"
#include "Pegasus/Texture/TextureGenerator.h"

namespace Pegasus {
namespace Texture {


TextureGeneratorProxy::TextureGeneratorProxy(TextureGenerator * generator)
:   mGenerator(generator)
{
    PG_ASSERTSTR(generator != nullptr, "Trying to create a texture generator proxy from an invalid texture generator object");
}

//----------------------------------------------------------------------------------------

TextureGeneratorProxy::~TextureGeneratorProxy()
{
}

//----------------------------------------------------------------------------------------
    
const char * TextureGeneratorProxy::GetName() const
{
    return mGenerator->GetName();
}


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
