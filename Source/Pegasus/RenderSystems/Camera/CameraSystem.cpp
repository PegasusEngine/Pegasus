/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CameraSystem.cpp
//! \author	Karolyn Boulanger
//! \date	04th September 2014
//! \brief	Camera manager, storing the global cameras

#include "Pegasus/RenderSystems/Camera/CameraSystem.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/Timeline/Timeline.h"

namespace Pegasus {
namespace Camera {


CameraSystem* gCameraSystem = nullptr;

CameraSystem::CameraSystem(Alloc::IAllocator * allocator)
:   mAllocator(allocator), RenderSystems::RenderSystem(allocator), mCamStateDirty(true)
{
    PG_ASSERTSTR(gCameraSystem == nullptr, "Should not instantiate two camera systems!");
    gCameraSystem = this;

    //initializing camera context vector
    int camCount = CAM_CONTEXT_COUNT.GetValue();
    for (int i = 0; i < camCount; ++i)
    {
        mCameras.PushEmpty() = nullptr;
    }
    mCurrentContext = CAM_WORLD_CONTEXT;

}

CameraSystem::~CameraSystem()
{
}

static void Camera_CreateCamera(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    Application::RenderCollection* collection = static_cast<Application::RenderCollection*>(context.GetVmState()->GetUserContext());
    Camera* cam = PG_NEW(Memory::GetRenderAllocator(), -1, "Camera alloc", Alloc::PG_MEM_TEMP) Camera(Memory::GetRenderAllocator());
    Application::RenderCollection::CollectionHandle handle = Application::RenderCollection::AddResource<Application::GenericResource>(collection, cam);
    stream.SubmitReturn<Application::RenderCollection::CollectionHandle>(handle);
}

static void Camera_BindCamera(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    Application::RenderCollection* collection = static_cast<Application::RenderCollection*>(context.GetVmState()->GetUserContext());
    Application::RenderCollection::CollectionHandle handle = stream.NextArgument<Application::RenderCollection::CollectionHandle>();
    CameraContext enumContextType(stream.NextArgument<int>());

    if (handle != Application::RenderCollection::INVALID_HANDLE)
    {
        Camera* cam = static_cast<Camera*>(Application::RenderCollection::GetResource<Application::GenericResource>(collection, handle));
      
        gCameraSystem->BindCamera(cam, enumContextType);
        
        //if bind camera is called within render, flush the camera into a cbuffer (means a new camera has been set).
        if (collection->GetRenderInfo() != nullptr)
        {
            const Timeline::RenderInfo* renderInfo = collection->GetRenderInfo();
            gCameraSystem->WindowUpdate(renderInfo->viewportWidth, renderInfo->viewportHeight);
        }
    }
    else
    {
        PG_LOG('ERR_', "Trying to set invalid camera!");
    }
}

static void Camera_UseCamContext(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    Application::RenderCollection* collection = static_cast<Application::RenderCollection*>(context.GetVmState()->GetUserContext());
    CameraContext enumContextType(stream.NextArgument<int>());
    if (enumContextType >= 0 && enumContextType < CAM_CONTEXT_COUNT.GetValue())
    {
        gCameraSystem->UseCameraContext(enumContextType);
        
        //if bind camera is called within render, flush the camera into a cbuffer (means a new camera has been set).
        if (collection->GetRenderInfo() != nullptr)
        {
            const Timeline::RenderInfo* renderInfo = collection->GetRenderInfo();
            gCameraSystem->WindowUpdate(renderInfo->viewportWidth, renderInfo->viewportHeight);
        }

    }
    else
    {
        PG_LOG('ERR_', "Trying to set invalid camera!");
    }
}

void CameraSystem::SetFreeCam(CameraRef freeCam)
{
    mCamStateDirty = mFreeCam != freeCam;
    mFreeCam = freeCam;
}

void CameraSystem::BindCamera(Camera* cam, CameraContext context)
{
    PG_ASSERT(context < static_cast<int>(mCameras.GetSize()));
    mCamStateDirty = mCameras[context.GetValue()] != cam;
    mCameras[context.GetValue()] = cam;
   
}

void CameraSystem::UseCameraContext(const CameraContext& context)
{
    PG_ASSERT(context < CAM_CONTEXT_COUNT.GetValue());
    mCamStateDirty = context != mCurrentContext;
    mCurrentContext = context;
}
    

void CameraSystem::Load(Core::IApplicationContext* appContext)
{
    mCameraBuffer = Render::CreateUniformBuffer(sizeof(Camera::GpuCamData));
}

void CameraSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{

    Application::GenericResource::RegisterGenericResourceType(Pegasus::Camera::Camera::GetStaticClassInfo(), blocklib); 

    BlockScript::FunctionDeclarationDesc extraFuns[3];
    extraFuns[0].functionName = "CreateCamera";
    extraFuns[0].returnType = "Camera";
    extraFuns[0].argumentTypes[0] = nullptr;
    extraFuns[0].argumentNames[0] = nullptr;
    extraFuns[0].callback = Camera_CreateCamera;

    extraFuns[1].functionName = "BindCamera";
    extraFuns[1].returnType = "int";
    extraFuns[1].argumentTypes[0] = "Camera";
    extraFuns[1].argumentNames[0] = "cam";
    extraFuns[1].argumentTypes[1] = "CameraContext";
    extraFuns[1].argumentNames[1] = "Context";
    extraFuns[1].argumentTypes[2] = "CameraContext";
    extraFuns[1].argumentNames[2] = "Context";
    extraFuns[1].argumentTypes[2] = nullptr;
    extraFuns[1].argumentNames[2] = nullptr;
    extraFuns[1].callback = Camera_BindCamera;

    extraFuns[2].functionName = "UseCameraContext";
    extraFuns[2].returnType = "int";
    extraFuns[2].argumentTypes[0] = "CameraContext";
    extraFuns[2].argumentNames[0] = "camContext"; 
    extraFuns[2].argumentTypes[1] = nullptr;
    extraFuns[2].argumentNames[1] = nullptr;    
    extraFuns[2].callback = Camera_UseCamContext;


    blocklib->CreateIntrinsicFunctions(extraFuns, sizeof(extraFuns)/sizeof(extraFuns[0]));
}

void CameraSystem::OnRegisterShaderGlobalConstants(Utils::Vector<ShaderGlobalConstantDesc>& outConstants)
{
    RenderSystem::ShaderGlobalConstantDesc& desc = outConstants.PushEmpty();
    desc.constantName = "__camera_cbuffer";
    desc.buffer = mCameraBuffer;
}

CameraRef CameraSystem::GetCurrentCamera()
{
    CameraRef currentCamera = (mFreeCam != nullptr && mCurrentContext == CAM_WORLD_CONTEXT) || mCameras[mCurrentContext.GetValue()] == nullptr ? mFreeCam : mCameras[mCurrentContext.GetValue()];
    return currentCamera;
}

void CameraSystem::WindowUpdate(unsigned int width, unsigned int height)
{
    CameraRef currentCamera = GetCurrentCamera();

    if (currentCamera != nullptr)
    {
        bool mustUpdate = currentCamera->WindowUpdate(width, height);
        if (mustUpdate || mCamStateDirty)
        {
            Pegasus::Render::SetBuffer(mCameraBuffer, &currentCamera->GetGpuData(), sizeof(Camera::GpuCamData));
            mCamStateDirty = false;
        }
    }

}

}   // namespace Camera
}   // namespace Pegasus

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif

