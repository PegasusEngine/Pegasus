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

#include "Pegasus/Render/Render.h"
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

    //! Creates a render target with the assigned texture configuration
    //! \param configuration of the render target
    //! \param output render target to fill / create
    void InternalCreateRenderTarget(Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& outputRenderTarget);

    //! Destroys a render target with the assigned configuration
    //! \param output render target to fill / create
    void InternalDeleteRenderTarget(Pegasus::Render::RenderTarget& renderTarget);

private:
    Pegasus::Render::OGLTextureGPUData * AllocateGPUData();
    void AllocateGPUData(Pegasus::Render::OGLTextureGPUData& output);
    void DeallocateTextureData(Pegasus::Render::OGLTextureGPUData& textureGPUData);
    Pegasus::Render::OGLTextureGPUData * GetGPUData(Pegasus::Texture::TextureData * textureData);
    Pegasus::Alloc::IAllocator * mAllocator;
};

void GLTextureFactory::AllocateGPUData(Pegasus::Render::OGLTextureGPUData& outputData)
{
    glGenTextures(1, &outputData.mHandle);
}

Pegasus::Render::OGLTextureGPUData * GLTextureFactory::AllocateGPUData()
{
    Pegasus::Render::OGLTextureGPUData * textureGPUData =
            PG_NEW(mAllocator,
                   -1,
                   "Texture GPU Data",
                   Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OGLTextureGPUData();

    AllocateGPUData(*textureGPUData);
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
        gpuData = AllocateGPUData();
        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(gpuData));
    }
    PG_ASSERT(gpuData != nullptr);
    GLint prevHandle = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevHandle);
    glBindTexture(GL_TEXTURE_2D, gpuData->mHandle);

    const Pegasus::Texture::TextureConfiguration& texConfig = nodeData->GetConfiguration();
    const unsigned char * texData = nodeData->GetLayerImageData(0);

    static const GLint GLPixelFormatTranslation[Pegasus::Texture::TextureConfiguration::NUM_PIXELFORMATS] =
    {
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
    glBindTexture(GL_TEXTURE_2D, (GLuint)prevHandle);
}

void GLTextureFactory::DeallocateTextureData(Pegasus::Render::OGLTextureGPUData& textureGPUData)
{
    glDeleteTextures(1, &textureGPUData.mHandle);
    textureGPUData.mHandle = 0; 
}

void GLTextureFactory::DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData)
{
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        Pegasus::Render::OGLTextureGPUData * textureData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLTextureGPUData, nodeData->GetNodeGPUData());

        DeallocateTextureData(*textureData);
        PG_DELETE(mAllocator, textureData);
        nodeData->SetNodeGPUData(nullptr);  // sets the dirty flag of the GPU node data
    }
    
}

void GLTextureFactory::InternalCreateRenderTarget(
    Pegasus::Render::RenderTargetConfig& config, 
    Pegasus::Render::RenderTarget& outputRenderTarget
)
{
    PG_ASSERT(outputRenderTarget.mInternalData == nullptr);

    Pegasus::Render::OGLRenderTargetGPUData * gpuData = 
            PG_NEW(mAllocator,
                   -1,
                   "Render Target GPU Data",
                   Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::OGLRenderTargetGPUData();
    //create internal texture names
    GLTextureFactory::AllocateGPUData(gpuData->mTextureView);
    GLint prevHandle = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevHandle);
    glBindTexture(GL_TEXTURE_2D, gpuData->mTextureView.mHandle);
    glTexImage2D(
        GL_TEXTURE_2D,
        0, //depth
        GL_RGBA, 
        config.mWidth,
        config.mHeight,
        0, //border, always 0
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    
    // Default filter, todo: create sampler
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

    //create frame buffer
    glGenFramebuffers(1, &gpuData->mFrameBufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, gpuData->mFrameBufferName);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gpuData->mTextureView.mHandle, 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    outputRenderTarget.mConfig = config;
    outputRenderTarget.mInternalData = reinterpret_cast<void*>(gpuData);
    PG_ASSERTSTR(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "something went really wrong in the creation of this!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, (GLuint)prevHandle);
}

void GLTextureFactory::InternalDeleteRenderTarget(
    Pegasus::Render::RenderTarget& renderTarget
)
{
    PG_ASSERT(renderTarget.mInternalData != nullptr);
    Pegasus::Render::OGLRenderTargetGPUData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLRenderTargetGPUData, renderTarget.mInternalData);
    glDeleteFramebuffers(1, &gpuData->mFrameBufferName);
    DeallocateTextureData(gpuData->mTextureView);
}

//! define a global static texture factory API
static GLTextureFactory gGlobalTextureFactory;

Pegasus::Texture::ITextureFactory * Pegasus::Render::GetRenderTextureFactory()
{
    return &gGlobalTextureFactory;
}

// SPECIAL RENDER API IMPLEMENTATIONS //
// These are the implementations that must be aware of the Texture Factory for allocations and other stuff

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATE RENDER TARGET IMPLEMENTATION /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::CreateRenderTarget(Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& renderTarget)
{
    gGlobalTextureFactory.InternalCreateRenderTarget(config, renderTarget);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
    gGlobalTextureFactory.InternalDeleteRenderTarget(renderTarget);
}
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif //PEGASUS_GAPI_GL
