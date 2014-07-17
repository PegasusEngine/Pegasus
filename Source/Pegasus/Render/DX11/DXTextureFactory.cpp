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
#include "Pegasus/Texture/TextureData.h"

class DXTextureFactory : public Pegasus::Texture::ITextureFactory
{
public:
    DXTextureFactory(){}
    virtual ~DXTextureFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);

    virtual void GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData);

    virtual void DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData);
};

void DXTextureFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
}

void DXTextureFactory::GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData)
{
}

void DXTextureFactory::DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData)
{
}

namespace Pegasus
{

namespace Render
{
DXTextureFactory gTextureFactory;
Texture::ITextureFactory * GetRenderTextureFactory()
{
    return &gTextureFactory;
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
