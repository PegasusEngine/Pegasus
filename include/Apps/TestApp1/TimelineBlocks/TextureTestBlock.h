/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTestBlock.h
//! \author	Karolyn Boulanger
//! \date	08th May 2014
//! \brief	Timeline block for the TextureTest effect (texture viewer)

#ifndef TESTAPP1_TEXTURETEST_BLOCK_H
#define TESTAPP1_TEXTURETEST_BLOCK_H

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

namespace Pegasus {
    namespace Timeline {
        struct UpdateInfo;
        struct RenderInfo;
    }
}

#if PEGASUS_ENABLE_RENDER_API
//! Timeline block for the TextureTest effect (texture viewer)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class TextureTestBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(TextureTestBlock, "TextureTest");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    TextureTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~TextureTestBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    virtual void Update(const Pegasus::Timeline::UpdateInfo& updateInfo) override;
    virtual void Render(const Pegasus::Timeline::RenderInfo& renderInfo) override;

    //------------------------------------------------------------------------------------

private:

    // Blocks cannot be copied
    PG_DISABLE_COPY(TextureTestBlock)

    //! Create the individual textures
    void CreateTexture1();
    void CreateTexture2();
    void CreateTextureGradient1();
    void CreateTextureGradient2();
    void CreateTextureAdd1();
    void CreateTextureAdd2();

    Pegasus::Shader::ProgramLinkageRef mProgram;
    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Render::Uniform mTextureUniform;
    Pegasus::Render::Uniform mTextureUniform2;
    Pegasus::Render::Uniform mUniformState;
    Pegasus::Render::BufferRef  mUniformBuffer;
    Pegasus::Render::SamplerStateRef mSampler;

    PEGASUS_ALIGN_BEGIN(16)
    struct UniformState
    {
        float screenRatio;
    } mState
    PEGASUS_ALIGN_END(16);

    Pegasus::Texture::TextureGeneratorRef mTextureGenerator1, mTextureGenerator2, mTextureGradientGenerator1, mTextureGradientGenerator2, mTexturePixelsGenerator1;
    Pegasus::Texture::TextureRef mTexture1, mTexture2, mTextureGradient1, mTextureGradient2, mTextureAdd1, mTextureAdd2;
    //GLuint mTextureSampler;
};
#endif

#endif  // TESTAPP1_TEXTURETEST_BLOCK_H
