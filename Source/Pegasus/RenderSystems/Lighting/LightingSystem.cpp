/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LightingSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/Lighting/LightingSystem.h"
#include "Pegasus/RenderSystems/Lighting/LightRig.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/BlockScript/FunCallback.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

using namespace Pegasus;
using namespace Pegasus::Application;
using namespace Pegasus::RenderSystems;
using namespace Pegasus::Lighting;

LightingSystem* gLightingSystemInstance = nullptr;

void LightRig_CreateLightRig(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    LightRig* newLightRig = PG_NEW(Memory::GetCoreAllocator(), -1, "LightRig", Pegasus::Alloc::PG_MEM_TEMP) LightRig(Memory::GetCoreAllocator());
    RenderCollection::CollectionHandle handle = RenderCollection::AddResource(collection, static_cast<GenericResource*>(newLightRig));
    stream.SubmitReturn<RenderCollection::CollectionHandle>(handle);
}

void LightRig_SetActiveLightRig(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle lightRigHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (lightRigHandle != RenderCollection::INVALID_HANDLE)
    {
        GenericResource* lightRigResource = RenderCollection::GetResource<GenericResource>(collection, lightRigHandle);
        LightRigRef lightRig = static_cast<LightRig*>(lightRigResource);
        gLightingSystemInstance->SetActiveLightRig(lightRig);
    }
    else
    {
        PG_LOG('ERR_', "Invalid light rig at SetActiveLightRig.");
    }
}

void LightRig_LoadCulledLightBuffer(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    //register first this buffer to this context
    Render::BufferRef culledLightBuffer = gLightingSystemInstance->GetCulledLightBuffer();
    stream.SubmitReturn( RenderCollection::AddResource<Render::Buffer>(collection, culledLightBuffer) );
}

void LightRig_GetActiveLightCount(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    //register first this buffer to this context
    stream.SubmitReturn( gLightingSystemInstance->GetActiveLightCount() );
}

template<class T>
void LightRig_AddLight(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    LightHandle retHandle = InvalidLightHandle;
    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        LightRig* lr = static_cast<LightRig*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        T& lightType = stream.NextArgument<T>();
        LightRig::Light l;
        l.SetState(lightType);
        retHandle = lr->AddLight(l);
    }
    else
    {
        PG_LOG('ERR_', "Invalid light rig pointer / handle in AddLight");
    }
    stream.SubmitReturn<LightHandle>(retHandle);
}

template<class T>
void LightRig_UpdateLight(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        LightRig* lr = static_cast<LightRig*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        LightHandle& lightHandle = stream.NextArgument<LightHandle>();
        T& lightType = stream.NextArgument<T>();
        LightRig::Light l;
        l.SetState(lightType);
        lr->UpdateLight(lightHandle, l);
    }
    else
    {
        PG_LOG('ERR_', "Invalid light rig pointer in UpdateLight");
    }
}

void LightRig_Clear(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();
    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        LightRig* lr = static_cast<LightRig*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        lr->Clear();
    }
    else
    {
        PG_LOG('ERR_', "Invalid light rig pointer / handle in Clear");
    }
}

void LightingSystem::Load(Core::IApplicationContext* appContext)
{
    PG_ASSERTSTR(gLightingSystemInstance == nullptr, "Can only have 1 instance of the lighting system.");
    gLightingSystemInstance = this;
    mLightBuffer = Render::CreateStructuredReadBuffer(sizeof(LightRig::GpuLightBuffer), LightRig::MaxLightsCount);
}

void LightingSystem::WindowUpdate(unsigned int width, unsigned int height)
{
    int bufferVersion = 0;
    unsigned int bufferByteSize = 0;
    if (mActiveLightRig != nullptr)
    {
        const LightRig::GpuLightBuffer& gpuLightBufferContents = mActiveLightRig->GetGpuBuffer(bufferByteSize, bufferVersion);

        if (bufferVersion != mCachedGpuLightBufferVersion)
        {
            Render::BeginMarker("CopyLightBuffer");
            Render::SetBuffer(mLightBuffer, &gpuLightBufferContents, (int)bufferByteSize);
            mCachedGpuLightBufferVersion = bufferVersion;
            Render::EndMarker();
        }
    }
}

int LightingSystem::GetActiveLightCount() const
{
    if (mActiveLightRig != nullptr)
    {
        return mActiveLightRig->GetLightCount();
    }
    return 0;
}

void LightingSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    //register simple structs of lights, make sure they match memory alignment with c++ sizes
    const BlockScript::StructDeclarationDesc lightStructDescs[] = {
        {
            "SphereLight",
            {"float4",           "float4",       "float",  nullptr},
            {"colorAndIntensity", "posAndRadius", "innerRadius", nullptr}
        },
        {
            "SpotLight",
            {"float4" ,            "float4",       "float4",       nullptr},
            {"colorAndIntensity", "posAndRadius", "dirAndSpread", nullptr}
        },
        {
            "LightHandle",
            { "int" , nullptr },
            { "_val", nullptr }
        }
    };

    blocklib->CreateStructTypes(lightStructDescs, sizeof(lightStructDescs)/sizeof(lightStructDescs[0]));

    //register methods of light rig
    const BlockScript::FunctionDeclarationDesc lightRigMethods[] = {
        {
            "AddLight",
            "LightHandle",
            { "LightRig", "SphereLight", nullptr },
            { "this", "sphereLight", nullptr },
            LightRig_AddLight<LightRig::SphereLight>
        },
        {
            "AddLight",
            "LightHandle",
            { "LightRig", "SpotLight", nullptr },
            { "this", "spotlight", nullptr },
            LightRig_AddLight<LightRig::SpotLight>
        },
        {
            "UpdateLight",
            "int",
            { "LightRig", "LightHandle" ,"SphereLight", nullptr },
            { "this", "handle", "sphereLight", nullptr },
            LightRig_UpdateLight<LightRig::SphereLight>
        },
        {
            "UpdateLight",
            "int",
            { "LightRig", "LightHandle", "SpotLight", nullptr },
            { "this", "handle", "spotlight", nullptr },
            LightRig_UpdateLight<LightRig::SpotLight>
        },
        {
            "Clear",
            "int",
            { "LightRig", nullptr },
            { "this", nullptr },
            LightRig_Clear
        }
    };

    Application::GenericResource::RegisterGenericResourceType(Pegasus::Lighting::LightRig::GetStaticClassInfo(), blocklib, lightRigMethods, sizeof(lightRigMethods)/sizeof(lightRigMethods[0]));

    const BlockScript::FunctionDeclarationDesc extraFuns[] = {
        {
            "CreateLightRig",
            "LightRig",
            { nullptr },
            { nullptr },
            LightRig_CreateLightRig
        },
        {
            "SetActiveLightRig",
            "int",
            { "LightRig", nullptr },
            { "lightrig", nullptr },
            LightRig_SetActiveLightRig
        },
        {
            "LoadCulledLightBuffer",
            "Buffer",
            { nullptr },
            { nullptr },
            LightRig_LoadCulledLightBuffer
        },
        {
            "GetActiveLightCount",
            "int",
            { nullptr },
            { nullptr },
            LightRig_GetActiveLightCount
        }
    };


    blocklib->CreateIntrinsicFunctions(extraFuns, sizeof(extraFuns)/sizeof(extraFuns[0]));
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
