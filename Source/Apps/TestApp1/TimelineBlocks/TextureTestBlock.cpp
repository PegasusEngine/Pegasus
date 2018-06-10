/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureTestBlock.cpp
//! \author	Karolyn Boulanger
//! \date	08th May 2014
//! \brief	Timeline block for the TextureTest effect (texture viewer)

#include "TimelineBlocks/TextureTestBlock.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Texture/Generator/ConstantColorGenerator.h"
#include "Pegasus/Texture/Generator/GradientGenerator.h"
#include "Pegasus/Texture/Generator/PixelsGenerator.h"
#include "Pegasus/Texture/Operator/AddOperator.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Math/Scalar.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Timeline/Timeline.h"

//----------------------------------------------------------------------------------------
#if PEGASUS_ENABLE_RENDER_API

TextureTestBlock::TextureTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
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

    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->LoadProgram("Programs/TextureTest.pas");

    // Force a compilation of the shaders
    bool updated = false;
    mProgram->GetUpdatedData(updated);

    Pegasus::Render::GetUniformLocation(
        mProgram,
        "inputTexture",
        mTextureUniform
    );

    Pegasus::Render::GetUniformLocation(
        mProgram,
        "inputTexture2",
        mTextureUniform2
    );

    Pegasus::Render::GetUniformLocation(
        mProgram,
        "uniformState",
        mUniformState
    );

    mUniformBuffer = Pegasus::Render::CreateUniformBuffer(
        sizeof(mState)
    );

    // Create the textures
    CreateTexture1();
    CreateTexture2();
    CreateTextureGradient1();
    CreateTextureGradient2();
    CreateTextureAdd1();
    CreateTextureAdd2();

    Pegasus::Render::SamplerStateConfig samplerConfig;
    mSampler = Pegasus::Render::CreateSamplerState(samplerConfig);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Shutdown()
{

}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Update(const Pegasus::Timeline::UpdateInfo& updateInfo)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mTexture1->Update();
    mTexture2->Update();
    mTextureGradient1->Update();
    mTextureGradient2->Update();
    mTextureAdd1->Update();
    mTextureAdd2->Update();
    mQuad->Update();
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::Render(const Pegasus::Timeline::RenderInfo& renderInfo)
{
    // Test dynamic data for the first gradient and the pixels generator
    Pegasus::Render::SetPixelSampler(mSampler, 0);
    Pegasus::Texture::GradientGenerator * gradientGenerator1 = static_cast<Pegasus::Texture::GradientGenerator *>(mTextureGradientGenerator1);
    Pegasus::Math::Vec3 & point0 = gradientGenerator1->GetPoint0();
    point0.y = Pegasus::Math::Sin(renderInfo.beat * 2.0f) * 0.5f + 0.5f;
    gradientGenerator1->SetPoint0(point0);
    Pegasus::Texture::PixelsGenerator * pixelsGenerator1 = static_cast<Pegasus::Texture::PixelsGenerator *>(mTexturePixelsGenerator1);
    pixelsGenerator1->SetNumPixels((unsigned int)(Pegasus::Math::Saturate((renderInfo.beat - 12.0f) / 3.0f) * 16384.0f));

    Pegasus::Render::SetProgram(mProgram);
    Pegasus::Render::SetMesh(mQuad);

    mState.screenRatio = renderInfo.aspect;
    Pegasus::Render::SetBuffer(mUniformBuffer, &mState);
    Pegasus::Render::SetUniformBuffer(mUniformState, mUniformBuffer);

    if (renderInfo.beat < 3.0f)
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTexture1);
    }
    else if (renderInfo.beat < 6.0f)
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTexture2);
    }
    else if (renderInfo.beat < 9.0f)
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTextureGradient1);
    }
    else if (renderInfo.beat < 12.0f)
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTextureGradient2);
    }
    else if (renderInfo.beat < 15.0f)
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTextureAdd1);
    }
    else
    {
        Pegasus::Render::SetUniformTexture(mTextureUniform, mTextureAdd2);
    }
    Pegasus::Render::SetUniformTexture(mTextureUniform2, mTextureAdd2);

    Pegasus::Render::Draw();
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTexture1()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   Pegasus::Core::FORMAT_RGBA_8_UNORM,
                                   256, 256, 1, 1);

    TextureGeneratorRef constantColorGenerator1Node = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    ConstantColorGenerator * constantColorGenerator1 = static_cast<ConstantColorGenerator *>(constantColorGenerator1Node);
    constantColorGenerator1->SetColor(Color8RGBA(132, 5, 212, 255));
    
    mTexture1 = textureManager->CreateTextureNode(texConfig);
    mTexture1->SetGeneratorInput(constantColorGenerator1Node);
    mTexture1->Update();

    mTextureGenerator1 = constantColorGenerator1Node;

#if PEGASUS_ENABLE_PROXIES
    mTexture1->SetName("Color1");
#endif
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTexture2()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   Pegasus::Core::FORMAT_RGBA_8_UNORM,
                                   256, 256, 1, 1);

    TextureGeneratorRef constantColorGenerator2Node = textureManager->CreateTextureGeneratorNode("ConstantColorGenerator", texConfig);
    ConstantColorGenerator * constantColorGenerator2 = static_cast<ConstantColorGenerator *>(constantColorGenerator2Node);
    constantColorGenerator2->SetColor(Color8RGBA(212, 5, 134, 255));

    mTexture2 = textureManager->CreateTextureNode(texConfig);
    mTexture2->SetGeneratorInput(constantColorGenerator2Node);
    mTexture2->Update();

    mTextureGenerator2 = constantColorGenerator2Node;

#if PEGASUS_ENABLE_PROXIES
    mTexture2->SetName("Color2");
#endif
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureGradient1()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   Pegasus::Core::FORMAT_RGBA_8_UNORM,
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

#if PEGASUS_ENABLE_PROXIES
    mTextureGradient1->SetName("Gradient1");
#endif

    // Object properties testing
    static const Pegasus::Math::Vec2 sDefaultVec2 = Pegasus::Math::Vec2(1.0f, 5.0f);
    static const Pegasus::Math::Color8RGBA sDefaultRGBA = Pegasus::Math::Color8RGBA(100, 120, 140, 255);
    mTextureGradient1->AddObjectProperty<Pegasus::Math::Vec2>("TestObjVec2", &sDefaultVec2);
    mTextureGradient1->AddObjectProperty<Pegasus::Math::Color8RGBA>("TestObjRGBA", &sDefaultRGBA);
}

//----------------------------------------------------------------------------------------

void TextureTestBlock::CreateTextureGradient2()
{
    using namespace Pegasus::Texture;
    using namespace Pegasus::Math;

    TextureManager* textureManager = GetTextureManager();
    TextureConfiguration texConfig(TextureConfiguration::TYPE_2D,
                                   Pegasus::Core::FORMAT_RGBA_8_UNORM,
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

#if PEGASUS_ENABLE_PROXIES
    mTextureGradient2->SetName("Gradient2");
#endif
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

    mTexturePixelsGenerator1 = textureManager->CreateTextureGeneratorNode("PixelsGenerator", texConfig);
    PixelsGenerator * pixelsGenerator1 = static_cast<PixelsGenerator *>(mTexturePixelsGenerator1);
    pixelsGenerator1->SetBackgroundColor(Color8RGBA(0, 32, 0, 255));
    pixelsGenerator1->SetColor0(Color8RGBA(0, 255, 0, 255));

    TextureOperatorRef addOperator2Node = textureManager->CreateTextureOperatorNode("AddOperator", texConfig);
    AddOperator * addOperator2 = static_cast<AddOperator *>(addOperator2Node);
    addOperator2->SetClamp(true);
    addOperator2Node->AddOperatorInput(addOperator1Node);
    addOperator2Node->AddGeneratorInput(mTexturePixelsGenerator1);

    mTextureAdd1 = textureManager->CreateTextureNode(texConfig);
    mTextureAdd1->SetOperatorInput(addOperator2Node);
    mTextureAdd1->Update();

#if PEGASUS_ENABLE_PROXIES
    mTextureAdd1->SetName("Add1");
#endif
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

#if PEGASUS_ENABLE_PROXIES
    mTextureAdd2->SetName("Add2");
#endif
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif