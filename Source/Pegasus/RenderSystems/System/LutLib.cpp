#include "Pegasus/RenderSystems/System/LutLib.h"
#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/Generator/TexCustomGenerator.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Application/RenderCollection.h"

using namespace Pegasus::Texture;
using namespace Pegasus::Render;

namespace Pegasus
{
namespace RenderSystems
{

void Callback_LoadAcosLut(Pegasus::BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    BlockScript::BsVmState* state = context.GetVmState();
    Pegasus::Application::RenderCollection* container = static_cast<Application::RenderCollection*>(state->GetUserContext());
    Pegasus::Core::IApplicationContext* appContext = container->GetAppContext();
    RenderSystemManager* rsManager = appContext->GetRenderSystemManager();
    LutLib* lutLib = rsManager->GetLutLib();
    Application::RenderCollection::CollectionHandle outputHandle = Application::RenderCollection::INVALID_HANDLE;
    if (lutLib)
    {
        Pegasus::Texture::TextureRef acosLut = lutLib->GetLut(LutLib::ACOS);
        Application::RenderCollection::CollectionHandle outputHandle = Application::RenderCollection::AddResource<Pegasus::Texture::Texture>(container, &(*acosLut));
    }
    stream.SubmitReturn<Application::RenderCollection::CollectionHandle>(outputHandle);
}

LutLib::LutLib(Alloc::IAllocator* allocator)
{
}

static Texture::TextureRef GenerateAcosLut(Pegasus::Texture::TextureManager* textureManager)
{
    unsigned lutResolution = 512;
    TextureConfiguration acosLutFmt(TextureConfiguration::TYPE_2D, Core::FORMAT_R32_FLOAT, lutResolution, 1, 1, 1);
    Pegasus::Texture::TextureGeneratorRef generatorRef = textureManager->CreateTextureGeneratorNode("TexCustomGenerator", acosLutFmt);
    Pegasus::Texture::TexCustomGenerator* acosLutGenerator = (Pegasus::Texture::TexCustomGenerator*)&(*generatorRef);
    TextureDataRef texData = acosLutGenerator->EditTextureData();
    unsigned char* imgDataRaw = texData->GetLayerImageData(0);
    float* imgData = reinterpret_cast<float*>(imgDataRaw);
    for (unsigned i = 0; i < lutResolution; ++i)
    {
        float cosAlpha = (float(i) / float(lutResolution - 1)) * 2.0f - 1.0f;
        imgData[i] = Math::Acos(cosAlpha);
    }
    Texture::TextureRef acosLut = textureManager->CreateTextureNode(acosLutFmt);
    acosLut->SetGeneratorInput(acosLutGenerator);
    acosLut->GetUpdatedTextureData(); //uploat to gpu
    return acosLut;

}
void LutLib::Load(Pegasus::Core::IApplicationContext* appContext)
{
    Pegasus::Texture::TextureManager* textureManager = appContext->GetTextureManager();
    mLuts.PushEmpty() = GenerateAcosLut(textureManager);

    RegisterBlockscript(appContext->GetRenderBsApi());
}

void LutLib::RegisterBlockscript(BlockScript::BlockLib* bsLib)
{
    BlockScript::FunctionDeclarationDesc fnDesc;
    fnDesc.argumentTypes[0] = nullptr;
    fnDesc.argumentNames[0] = nullptr;
    fnDesc.returnType = "Texture";
    // ------------------------------

    fnDesc.functionName = "LoadAcosLut";
    fnDesc.callback = Callback_LoadAcosLut;

    // ------------------------------
    bsLib->CreateIntrinsicFunctions(&fnDesc, 1);
}


}
}

