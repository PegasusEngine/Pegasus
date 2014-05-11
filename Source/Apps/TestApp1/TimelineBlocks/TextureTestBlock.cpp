/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTestBlock.cpp
//! \author	Kevin Boulanger
//! \date	08th May 2014
//! \brief	Timeline block for the TextureTest effect (texture viewer)

#include "TimelineBlocks/TextureTestBlock.h"
#include "Pegasus/Render/Render.h"


static const GLuint NUM_VERTS = 6;
static const GLfloat verts[NUM_VERTS][2] =
    {
        { -1.0f, -1.0f },
        {  1.0f, -1.0f },
        { -1.0f,  1.0f },
        { -1.0f,  1.0f },
        {  1.0f, -1.0f },
        {  1.0f,  1.0f }
    };
enum ATTRIB_IDS { POSITION_ATTRIB = 0 };

static const char * VERTEX_SHADER = "Shaders\\TextureTest.vs";
static const char * FRAGMENT_SHADER = "Shaders\\TextureTest.ps";

//----------------------------------------------------------------------------------------
    
TextureTestBlock::TextureTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
:   Pegasus::Timeline::Block(allocator, appContext)
{
}

//----------------------------------------------------------------------------------------

TextureTestBlock::~TextureTestBlock()
{
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Initialize()
{
    // Create and bind vertex array
    glGenVertexArrays(NUM_VAOS, mVAOs);
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);

    // Create and fill buffers
    glGenBuffers(NUM_BUFFERS, mBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TRIANGLES_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Immutable verts

    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->CreateProgram("TextureTest");
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();

    fileLoadProperties.mPath = VERTEX_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    fileLoadProperties.mPath = FRAGMENT_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    // Force a compilation of the shaders
    bool updated = false;
    mProgram->GetUpdatedData(updated);

    // Set up shader uniforms
    mScreenRatioUniform = 1;//glGetUniformLocation(mProgramData->GetHandle(), "screenRatio");
    mTextureUniform = 0; //glGetUniformLocation(mProgramData->GetHandle(), "inputTexture");

    // Bind vertex array to shader
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); // 2 floats, non-normalized, 0 stride and offset
    glEnableVertexAttribArray(POSITION_ATTRIB);

    // Create the textures
    CreateTexture1();
    CreateTexture2();

    // Create the sampler object
    //mTextureSampler = 0;
    //glGenSamplers(1, &mTextureSampler);
    //glBindSampler(0, mTextureSampler);
    //glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::Dispatch(mProgram);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    // Set up and draw triangles
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);
    glUniform1f(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));

    glActiveTexture(GL_TEXTURE0);
    if (beat < 6.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mTexture1);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, mTexture2);
    }
    glUniform1i(mTextureUniform, 0);
    //glBindSampler(0, mTextureSampler);

    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
}

//----------------------------------------------------------------------------------------

GLuint TextureTestBlock::CreateGLTexture(Pegasus::Texture::TextureRef texture)
{
    GLuint textureHandle = 0;
    glGenTextures(1, &textureHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const Pegasus::Texture::TextureConfiguration & texConfig = texture->GetConfiguration();
    const unsigned char * texData = texture->GetUpdatedTextureData()->GetLayerImageData(0);
    if (texConfig.GetPixelFormat() == Pegasus::Texture::TextureConfiguration::PIXELFORMAT_RGBA8)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     texConfig.GetWidth(), texConfig.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     texData);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     texConfig.GetWidth(), texConfig.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                     texData);
    }

    return textureHandle;
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTexture1()
{
    using namespace Pegasus::Texture;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureRef texture1 = textureManager->CreateTextureNode(texConfig);
    //TextureOperatorRef addOperator1 = textureManager->CreateTextureOperatorNode("AddOperator", texConfig);
    TextureGeneratorRef constantColorGenerator1 = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    //TextureGeneratorRef constantColorGenerator2 = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);

    //addOperator1->AddGeneratorInput(constantColorGenerator1);
    //addOperator1->AddGeneratorInput(constantColorGenerator2);
    //texture1->SetOperatorInput(addOperator1);
    texture1->SetGeneratorInput(constantColorGenerator1);

    texture1->Update();
    mTexture1 = CreateGLTexture(texture1);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTexture2()
{
    using namespace Pegasus::Texture;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureRef texture2 = textureManager->CreateTextureNode(texConfig);
    //TextureOperatorRef addOperator1 = textureManager->CreateTextureOperatorNode("AddOperator", texConfig);
    TextureGeneratorRef constantColorGenerator1 = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    //TextureGeneratorRef constantColorGenerator2 = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);

    //addOperator1->AddGeneratorInput(constantColorGenerator1);
    //addOperator1->AddGeneratorInput(constantColorGenerator2);
    //texture1->SetOperatorInput(addOperator1);
    texture2->SetGeneratorInput(constantColorGenerator1);

    texture2->Update();
    mTexture2 = CreateGLTexture(texture2);
}
