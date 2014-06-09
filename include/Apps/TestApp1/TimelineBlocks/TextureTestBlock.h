/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTestBlock.h
//! \author	Kevin Boulanger
//! \date	08th May 2014
//! \brief	Timeline block for the TextureTest effect (texture viewer)

#ifndef TESTAPP1_TEXTURETEST_BLOCK_H
#define TESTAPP1_TEXTURETEST_BLOCK_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"

//! Timeline block for the TextureTest effect (texture viewer)
//! \todo Use protected inheritance? In that case, give access to SetPosition() and SetLength()
class TextureTestBlock : public Pegasus::Timeline::Block
{
    DECLARE_TIMELINE_BLOCK(TextureTestBlock, "TextureTest");

public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    TextureTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~TextureTestBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Pegasus::Wnd::Window * window);

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
    Pegasus::Render::Uniform mScreenRatioUniform;
    Pegasus::Render::Uniform mTextureUniform;
    Pegasus::Render::Uniform mTextureUniform2;
    Pegasus::Texture::TextureGeneratorRef mTextureGenerator1, mTextureGenerator2, mTextureGradientGenerator1, mTextureGradientGenerator2;
    Pegasus::Texture::TextureRef mTexture1, mTexture2, mTextureGradient1, mTextureGradient2, mTextureAdd1, mTextureAdd2;
    //GLuint mTextureSampler;
};


#endif  // TESTAPP1_TEXTURETEST_BLOCK_H
