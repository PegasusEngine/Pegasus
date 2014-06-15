/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLTextureFactory.cpp
//! \author Kleber Garcia
//! \date   1st June 2014
//! \brief  Texture factory implementation. Provided to the texture package

#if PEGASUS_GAPI_GL

#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Texture/Texture.h"

//! internal definition of texture factory API
class GLTextureFactory : public Pegasus::Texture::ITextureFactory
{
public:
    GLTextureFactory() : mAllocator(nullptr){}

    virtual ~GLTextureFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator)
    {
        mAllocator = allocator;
    }

    //! Generates GPU data for a texture data node. 
    //! \param nodeData 
    virtual void GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData); 

    //! Destroys GPU data for a texture data node. 
    //! \param nodeData 
    virtual void DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData);

private:
    Pegasus::Render::OGLTextureGPUData * AllocateGPUData(Pegasus::Texture::TextureData * textureData);
    Pegasus::Render::OGLTextureGPUData * GetGPUData(Pegasus::Texture::TextureData * textureData);
    Pegasus::Alloc::IAllocator * mAllocator;
};

Pegasus::Render::OGLTextureGPUData * GLTextureFactory::AllocateGPUData(Pegasus::Texture::TextureData * nodeData)
{
    PG_ASSERT(nodeData != nullptr);
    PG_ASSERT(nodeData->GetNodeGPUData() == nullptr);

    Pegasus::Render::OGLTextureGPUData * textureGPUData =
            PG_NEW(mAllocator,
                   -1,
                   "Texture GPU Data",
                   Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OGLTextureGPUData();

    glGenTextures(1, &textureGPUData->mHandle);
    nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(textureGPUData));

    return textureGPUData;
}

Pegasus::Render::OGLTextureGPUData * GLTextureFactory::GetGPUData(Pegasus::Texture::TextureData * nodeData)
{
    PG_ASSERT(nodeData != nullptr);
    PG_ASSERT(nodeData->GetNodeGPUData() != nullptr);

    return PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLTextureGPUData, nodeData->GetNodeGPUData());
}

void GLTextureFactory::GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData)
{
    Pegasus::Render::OGLTextureGPUData * gpuData = nullptr;
    bool newlyAllocated = false;
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        gpuData = GetGPUData(nodeData);
    }
    else
    {
        newlyAllocated = true;
        gpuData = AllocateGPUData(nodeData);
    }
    PG_ASSERT(gpuData != nullptr);
    
    glBindTexture(GL_TEXTURE_2D, gpuData->mHandle);

    const Pegasus::Texture::TextureConfiguration& texConfig = nodeData->GetConfiguration();
    const unsigned char * texData = nodeData->GetLayerImageData(0);

    static const GLint GLPixelFormatTranslation[Pegasus::Texture::TextureConfiguration::NUM_PIXELFORMATS] =
    {
        GL_RGB,
        GL_RGBA
    };

    //! \todo Support all texture types
    PG_ASSERTSTR(texConfig.GetType() == Pegasus::Texture::TextureConfiguration::TYPE_2D,
                 "Unsupported texture format. Only 2D textures are supported for the moment");

    if (newlyAllocated)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0, 
            GLPixelFormatTranslation[texConfig.GetPixelFormat()],
            texConfig.GetWidth(),
            texConfig.GetHeight(),
            0,
            GLPixelFormatTranslation[texConfig.GetPixelFormat()],
            GL_UNSIGNED_BYTE,
            texData);
    }
    else
    {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            texConfig.GetWidth(),
            texConfig.GetHeight(),
            GLPixelFormatTranslation[texConfig.GetPixelFormat()],
            GL_UNSIGNED_BYTE,
            texData);
    }

    // Default filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Since the texture data has been updated, set the node GPU data as non-dirty
    nodeData->ValidateGPUData();
}

void GLTextureFactory::DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData)
{
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        Pegasus::Render::OGLTextureGPUData * textureData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLTextureGPUData, nodeData->GetNodeGPUData());
        glDeleteTextures(1, &textureData->mHandle);
        PG_DELETE(mAllocator, textureData);
        nodeData->SetNodeGPUData(nullptr);  // sets the dirty flag of the GPU node data
    }
    
}

//! define a global static texture factory API
static GLTextureFactory gGlobalTextureFactory;

Pegasus::Texture::ITextureFactory * Pegasus::Render::GetRenderTextureFactory()
{
    return &gGlobalTextureFactory;
}

#endif //PEGASUS_GAPI_GL
