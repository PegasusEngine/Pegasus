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
#include "Pegasus/Texture/Generator/ConstantColorGenerator.h"
#include "Pegasus/Texture/Generator/GradientGenerator.h"
#include "Pegasus/Texture/Operator/AddOperator.h"

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
    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);
    mQuad->GetUpdatedMeshData();

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

    // Create the textures
    CreateTexture1();
    CreateTexture2();
    CreateTextureGradient1();
    CreateTextureGradient2();
    CreateTextureAdd1();
    CreateTextureAdd2();

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
    Pegasus::Render::Dispatch(mQuad);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    glUniform1f(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));

    glActiveTexture(GL_TEXTURE0);
    if (beat < 3.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mGLTexture1);
    }
    else if (beat < 6.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mGLTexture2);
    }
    else if (beat < 9.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mGLTextureGradient1);
    }
    else if (beat < 12.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mGLTextureGradient2);
    }
    else if (beat < 15.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mGLTextureAdd1);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, mGLTextureAdd2);
    }
    glUniform1i(mTextureUniform, 0);
    //glBindSampler(0, mTextureSampler);

    Pegasus::Render::Draw();
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
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureGeneratorRef constantColorGenerator1Node = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    ConstantColorGenerator * constantColorGenerator1 = static_cast<ConstantColorGenerator *>(constantColorGenerator1Node);
    constantColorGenerator1->SetColor(Color8RGBA(132, 5, 212, 255));
    
    mTexture1 = textureManager->CreateTextureNode(texConfig);
    mTexture1->SetGeneratorInput(constantColorGenerator1Node);
    mTexture1->Update();

    mTextureGenerator1 = constantColorGenerator1Node;

    mGLTexture1 = CreateGLTexture(mTexture1);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTexture2()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureGeneratorRef constantColorGenerator2Node = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    ConstantColorGenerator * constantColorGenerator2 = static_cast<ConstantColorGenerator *>(constantColorGenerator2Node);
    constantColorGenerator2->SetColor(Color8RGBA(212, 5, 134, 255));

    mTexture2 = textureManager->CreateTextureNode(texConfig);
    mTexture2->SetGeneratorInput(constantColorGenerator2Node);
    mTexture2->Update();

    mTextureGenerator2 = constantColorGenerator2Node;

    mGLTexture2 = CreateGLTexture(mTexture2);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureGradient1()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureGeneratorRef gradientGenerator1Node = textureManager->CreateTextureGeneratorNode("GradientGenerator", texConfig);
    GradientGenerator * gradientGenerator1 = static_cast<GradientGenerator *>(gradientGenerator1Node);
    gradientGenerator1->SetColor0(Color8RGBA(32, 32, 64, 255));
    gradientGenerator1->SetColor1(Color8RGBA(192, 32, 0, 255));
    gradientGenerator1->SetPoint0(Vec3(0.2f, 0.5f, 0.0f));
    gradientGenerator1->SetPoint1(Vec3(0.3f, 0.7f, 0.0f));
    
    mTextureGradient1 = textureManager->CreateTextureNode(texConfig);
    mTextureGradient1->SetGeneratorInput(gradientGenerator1Node);
    mTextureGradient1->Update();

    mTextureGradientGenerator1 = gradientGenerator1Node;

    mGLTextureGradient1 = CreateGLTexture(mTextureGradient1);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureGradient2()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   TextureConfiguration::PIXELFORMAT_RGBA8,
                                   256, 256, 1, 1);

    TextureGeneratorRef gradientGenerator2Node = textureManager->CreateTextureGeneratorNode("GradientGenerator", texConfig);
    GradientGenerator * gradientGenerator2 = static_cast<GradientGenerator *>(gradientGenerator2Node);
    gradientGenerator2->SetColor0(Color8RGBA(32, 64, 32, 255));
    gradientGenerator2->SetColor1(Color8RGBA(0, 192, 128, 255));
    gradientGenerator2->SetPoint0(Vec3(0.7f, 0.7f, 0.0f));
    gradientGenerator2->SetPoint1(Vec3(0.5f, 0.9f, 0.0f));
    
    mTextureGradient2 = textureManager->CreateTextureNode(texConfig);
    mTextureGradient2->SetGeneratorInput(gradientGenerator2Node);
    mTextureGradient2->Update();

    mTextureGradientGenerator2 = gradientGenerator2Node;

    mGLTextureGradient2 = CreateGLTexture(mTextureGradient2);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureAdd1()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    const TextureConfiguration & texConfig = mTexture1->GetConfiguration();

    TextureOperatorRef addOperator1Node = textureManager->CreateTextureOperatorNode("AddOperator", texConfig);
    AddOperator * addOperator1 = static_cast<AddOperator *>(addOperator1Node);
    addOperator1->SetClamp(true);
    addOperator1Node->AddGeneratorInput(mTextureGenerator1);
    addOperator1Node->AddGeneratorInput(mTextureGradientGenerator1);
    addOperator1Node->AddGeneratorInput(mTextureGradientGenerator2);

    mTextureAdd1 = textureManager->CreateTextureNode(texConfig);
    mTextureAdd1->SetOperatorInput(addOperator1Node);
    mTextureAdd1->Update();

    mGLTextureAdd1 = CreateGLTexture(mTextureAdd1);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureAdd2()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    const TextureConfiguration & texConfig = mTexture1->GetConfiguration();

    TextureOperatorRef addOperator2Node = textureManager->CreateTextureOperatorNode("AddOperator", texConfig);
    AddOperator * addOperator2 = static_cast<AddOperator *>(addOperator2Node);
    addOperator2->SetClamp(false);
    addOperator2Node->AddGeneratorInput(mTextureGenerator1);
    addOperator2Node->AddGeneratorInput(mTextureGradientGenerator1);
    addOperator2Node->AddGeneratorInput(mTextureGradientGenerator2);

    mTextureAdd2 = textureManager->CreateTextureNode(texConfig);
    mTextureAdd2->SetOperatorInput(addOperator2Node);
    mTextureAdd2->Update();

    mGLTextureAdd2 = CreateGLTexture(mTextureAdd2);
}