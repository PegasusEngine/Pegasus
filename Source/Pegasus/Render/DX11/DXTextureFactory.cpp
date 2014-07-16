/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXTextureFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 texture factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/Render.h"
#include "Pegasus/Render/TextureFactory.h"

namespace Pegasus
{

namespace Render
{

Texture::ITextureFactory * GetRenderTextureFactory()
{
    return nullptr;
}

}

}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATE RENDER TARGET IMPLEMENTATION /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::CreateRenderTarget(Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& renderTarget)
{
    
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
