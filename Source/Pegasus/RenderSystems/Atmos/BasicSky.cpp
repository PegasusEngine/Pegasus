/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BasicSky.cpp
//! \author Kleber Garcia
//! \date   January 14th 2018
//! \brief  A basic procedural sky.

#include "Pegasus/RenderSystems/Atmos/BasicSky.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/RenderSystems/Atmos/AtmosSystem.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Application/RenderCollection.h"

using namespace Pegasus;
using namespace Pegasus::Application;
using namespace Pegasus::Shader;
using namespace Pegasus::Render;
using namespace Pegasus::Mesh;
using namespace Pegasus::Math;
using namespace Pegasus::RenderSystems;

extern AtmosSystem* gAtmosSystemInstance;

namespace Pegasus 
{
namespace RenderSystems
{

BEGIN_IMPLEMENT_PROPERTIES(BasicSky)
    IMPLEMENT_PROPERTY(BasicSky, Visible)
    IMPLEMENT_PROPERTY(BasicSky, Distance)
    IMPLEMENT_PROPERTY(BasicSky, CubeMapEnable)
    IMPLEMENT_PROPERTY(BasicSky, CubeMapResolution)
    IMPLEMENT_PROPERTY(BasicSky, CubeMapVersion)
END_IMPLEMENT_PROPERTIES(BasicSky)

BasicSky::BasicSky(Alloc::IAllocator* allocator)
: Application::GenericResource(allocator)
{
    BEGIN_INIT_PROPERTIES(BasicSky)
        INIT_PROPERTY(Visible)
        INIT_PROPERTY(Distance)
        INIT_PROPERTY(CubeMapEnable)
        INIT_PROPERTY(CubeMapResolution)
        INIT_PROPERTY(CubeMapVersion)
    END_INIT_PROPERTIES()

    CubeMapConfig cmConfig;
    cmConfig.mWidth = GetCubeMapResolution();
    cmConfig.mHeight = GetCubeMapResolution();
    cmConfig.mFormat = Core::FORMAT_RGBA_16_FLOAT;
    mSkyCubeMap = CreateCubeMap(cmConfig);
    mEnvmapCachedVersion = 0;
    for (unsigned int i = 0; i < Render::CUBE_FACE_COUNT; ++i)
    {
        mSkyCubeTargets[i] = CreateRenderTargetFromCubeMap(static_cast<Render::CubeFace>(i), mSkyCubeMap);
    }
}

static void CreateBasicSky_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    BasicSky* newSky = PG_NEW(Memory::GetCoreAllocator(), -1, "BasicSky", Pegasus::Alloc::PG_MEM_TEMP) BasicSky(Memory::GetCoreAllocator());
    RenderCollection::CollectionHandle handle = RenderCollection::AddResource<GenericResource>(collection, newSky);
    stream.SubmitReturn<RenderCollection::CollectionHandle>(handle);
}

static void DrawBasicSky_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        BasicSky* basicSky = static_cast<BasicSky*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        basicSky->Draw();
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid sky.");
    }
}

static void LoadSkyCubeMap_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    RenderCollection::CollectionHandle returnHandle = RenderCollection::INVALID_HANDLE;
    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        BasicSky* basicSky = static_cast<BasicSky*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        Render::CubeMapRef cm = basicSky->GetSkyCubeMap();
        returnHandle = RenderCollection::AddResource<Render::CubeMap>(collection, cm);
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid sky.");
    }
    stream.SubmitReturn<RenderCollection::CollectionHandle>(returnHandle);
}

void BasicSky::Update()
{
    DrawUpdate();
}

void BasicSky::DrawUpdate()
{
    //Predraw cube map
    if (GetCubeMapVersion() != mEnvmapCachedVersion)
    {
    
        mEnvmapCachedVersion = GetCubeMapVersion();
    }
}

void BasicSky::Draw()
{
    if (GetVisible())
    {
        gAtmosSystemInstance->DrawBasicSky(this);
    }
}

void BasicSky::GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions)
{
    BlockScript::FunctionDeclarationDesc desc;
    desc.functionName = "CreateBasicSky";
    desc.returnType = "BasicSky";
    desc.argumentTypes[0] = nullptr;
    desc.argumentNames[0] = nullptr;
    desc.callback = CreateBasicSky_Callback;
    functions.PushEmpty() = desc; 

    desc.functionName = "Draw";
    desc.returnType = "int";
    desc.argumentTypes[0] = "BasicSky";
    desc.argumentNames[0] = "this";
    desc.argumentTypes[1] = nullptr;
    desc.argumentNames[1] = nullptr;
    desc.callback = DrawBasicSky_Callback;
    methods.PushEmpty() = desc;

    desc.functionName = "LoadSkyCubeMap";
    desc.returnType = "CubeMap";
    desc.callback = LoadSkyCubeMap_Callback;
    methods.PushEmpty() = desc;
}

}
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
