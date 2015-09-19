/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.cpp
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Application/ScriptRenderApi.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/AppWindowManager.h"
#include "Pegasus/Application/AppBsReflectionInfo.h"
#include "Pegasus/Application/AppWindowComponentFactory.h"
#include "Pegasus/Camera/CameraManager.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Render/IDevice.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/TimelineBlock/TimelineBlockRegistration.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Render/RenderContext.h"

#include <stdio.h>

namespace Pegasus {
namespace App {

//----------------------------------------------------------------------------------------

Application::Application(const ApplicationConfig& config)
{
    Alloc::IAllocator* coreAlloc = Memory::GetCoreAllocator();
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Alloc::IAllocator* nodeAlloc = Memory::GetNodeAllocator();
    Alloc::IAllocator* nodeDataAlloc = Memory::GetNodeDataAllocator();
    Alloc::IAllocator* timelineAlloc = Memory::GetTimelineAllocator();

    AppWindowManagerConfig windowManagerConfig;

    // Set up debugging facilities
#if PEGASUS_ENABLE_LOG
    Core::LogManager::CreateInstance(coreAlloc);
    Core::LogManager::GetInstance()->RegisterHandler(config.mLoghandler);
#endif
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::CreateInstance(coreAlloc);
    Core::AssertionManager::GetInstance()->RegisterHandler(config.mAssertHandler);
#endif

    mComponentFactory = PG_NEW(coreAlloc, -1, "ComponentFactory", Alloc::PG_MEM_PERM) AppWindowComponentFactory(coreAlloc);

    // Set up the hierarchy of property grid class metadata gathered at compile time
    PropertyGrid::PropertyGridManager::GetInstance().ResolveInternalClassHierarchy();
    mPropertyGridManager = &PropertyGrid::PropertyGridManager::GetInstance();

    // Set up the time system
    Core::InitializePegasusTime();


    // Set up window manager
    windowManagerConfig.mAllocator = windowAlloc;
    windowManagerConfig.mMaxWindowTypes = config.mMaxWindowTypes;
    windowManagerConfig.mMaxNumWindows = config.mMaxNumWindows;
    mWindowManager = PG_NEW(windowAlloc, -1, "AppWindowManager", Alloc::PG_MEM_PERM) AppWindowManager(windowManagerConfig);


    // Set up node managers
    mNodeManager = PG_NEW(nodeAlloc, -1, "NodeManager", Alloc::PG_MEM_PERM) Graph::NodeManager(nodeAlloc, nodeDataAlloc);

    Pegasus::Shader::IShaderFactory * shaderFactory = Pegasus::Render::GetRenderShaderFactory();
    Pegasus::Mesh::IMeshFactory * meshFactory = Pegasus::Render::GetRenderMeshFactory();
    Pegasus::Texture::ITextureFactory * textureFactory = Pegasus::Render::GetRenderTextureFactory();

    //! TODO - we probably need to use a render specific allocator for this
    shaderFactory->Initialize(nodeDataAlloc);
    meshFactory->Initialize(nodeDataAlloc);
    textureFactory->Initialize(nodeDataAlloc);

    
    // Set up asset library
    mAssetLib = PG_NEW(nodeAlloc, -1, "AssetLib", Alloc::PG_MEM_PERM) AssetLib::AssetLib(nodeAlloc, nullptr);

    mShaderManager = PG_NEW(nodeAlloc, -1, "ShaderManager", Alloc::PG_MEM_PERM) Shader::ShaderManager(mNodeManager, shaderFactory);
    mTextureManager = PG_NEW(nodeAlloc, -1, "TextureManager", Alloc::PG_MEM_PERM) Texture::TextureManager(mNodeManager, textureFactory);
    mMeshManager = PG_NEW(nodeAlloc, -1, "MeshManager", Alloc::PG_MEM_PERM) Mesh::MeshManager(mNodeManager, meshFactory);
    mBlockScriptManager = PG_NEW(timelineAlloc, -1, "BlockScript Manager", Alloc::PG_MEM_PERM) BlockScript::BlockScriptManager(timelineAlloc);
    mTimelineManager = PG_NEW(timelineAlloc, -1, "Timeline Manager", Alloc::PG_MEM_PERM) Timeline::TimelineManager(timelineAlloc, this);

    mAssetLib->RegisterObjectFactory(mShaderManager);
    mAssetLib->RegisterObjectFactory(mTimelineManager);
    //mAssetLib->RegisterObjectFactory(mTextureManager);
    //mAssetLib->RegisterObjectFactory(mMeshManager);

    mRenderCollectionFactory = PG_NEW(nodeAlloc, -1, "RenderCollectionFactory", Alloc::PG_MEM_PERM) Pegasus::Application::RenderCollectionFactory(this, nodeAlloc);
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    mBsReflectionInfo = PG_NEW(nodeAlloc, -1, "Bs Reflection Info", Alloc::PG_MEM_PERM) App::AppBsReflectionInfo(nodeAlloc);
#endif

    //register shader manager into factory, so factory can handle includes
    shaderFactory->RegisterShaderManager(mShaderManager);

    // Register the entire render api
    Pegasus::Application::RegisterRenderApi(mBlockScriptManager->GetRuntimeLib(), this);

#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    mBsReflectionInfo->RegisterLib(mBlockScriptManager->GetRuntimeLib());
#endif
    
	// Create the owner of all global cameras
    mCameraManager = PG_NEW(timelineAlloc, -1, "CameraManager", Alloc::PG_MEM_PERM) Camera::CameraManager(timelineAlloc);

    // Cache config
    mConfig = config;

    Wnd::Window* startupWindow = nullptr;

    // Set up IO manager
    // This must be done here because of the GetAppName virtual
    char rootPath[Io::IOManager::MAX_FILEPATH_LENGTH];
    sprintf_s(rootPath, Io::IOManager::MAX_FILEPATH_LENGTH - 1, "%s\\Imported\\", mConfig.mBasePath); // Hardcode imported for now
    mIoManager = PG_NEW(coreAlloc, -1, "IOManager", Pegasus::Alloc::PG_MEM_PERM) Io::IOManager(rootPath);
    
    mAssetLib->SetIoManager(mIoManager); //TODO: decide here if we use the pakIoManager or the standard file system IOManager

    // Start up the app, which creates and destroys the dummy window
    StartupAppInternal();

    // Register the Pegasus-side timeline blocks
    TimelineBlock::RegisterBaseBlocks(GetTimelineManager());

}

//----------------------------------------------------------------------------------------

Application::~Application()
{

    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Alloc::IAllocator* nodeAlloc = Memory::GetNodeAllocator();
    Alloc::IAllocator* nodeDataAlloc = Memory::GetNodeDataAllocator();
    Alloc::IAllocator* timelineAlloc = Memory::GetTimelineAllocator();
    Alloc::IAllocator* coreAlloc  = Memory::GetCoreAllocator();

    PG_DELETE(windowAlloc, mWindowManager);

    // Delete the timeline
    PG_DELETE(timelineAlloc, mTimelineManager);

    // Delete the texture and node managers    
    PG_DELETE(nodeAlloc, mAssetLib);
    PG_DELETE(nodeAlloc, mMeshManager);
    PG_DELETE(nodeAlloc, mTextureManager);
    PG_DELETE(nodeAlloc, mShaderManager);
    PG_DELETE(nodeAlloc, mNodeManager);
    PG_DELETE(nodeAlloc, mRenderCollectionFactory);
    PG_DELETE(timelineAlloc, mBlockScriptManager);
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    PG_DELETE(nodeAlloc, mBsReflectionInfo);
#endif

    PG_DELETE(coreAlloc, mComponentFactory);

    // Tear down debugging facilities
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::GetInstance()->UnregisterHandler();
    Core::AssertionManager::DestroyInstance();
#endif
#if PEGASUS_ENABLE_LOG
    Core::LogManager::GetInstance()->UnregisterHandler();
    Core::LogManager::DestroyInstance();
#endif
}

//----------------------------------------------------------------------------------------

void Application::Initialize()
{
    // Custom initialization, done in the user application
    RegisterTimelineBlocks();
}

//----------------------------------------------------------------------------------------

//! Shutdown this application.
void Application::Shutdown()
{
    Alloc::IAllocator* coreAlloc = Memory::GetCoreAllocator();

    // Custom shutdown, done in the user application
    ShutdownApp();

    // Shuts down GLextensions, etc
    ShutdownAppInternal();

    // Tear down IO manager
    PG_DELETE(coreAlloc, mIoManager);
}

//----------------------------------------------------------------------------------------

void Application::Load()
{
    
    
    Render::ContextConfig config;
    config.mAllocator = Memory::GetCoreAllocator();
    config.mDevice = mDevice;
    Render::Context context(config);
    context.Bind();
    
    //! Call custom initialize here
    InitializeApp();

    // Initialize all the components for all the windows.
    mComponentFactory->LoadAllComponents(this);

    context.Unbind();
    
}

//----------------------------------------------------------------------------------------

void Application::Update()
{
    //! update all components, globally for all the windows.
    mComponentFactory->UpdateAllComponents(this);
}

//----------------------------------------------------------------------------------------

void Application::Unload()
{
    Render::ContextConfig config;
    config.mAllocator = Memory::GetCoreAllocator();
    config.mDevice = mDevice;
    Render::Context context(config);
    context.Bind();

    // Initialize all the components for all the windows.
    mComponentFactory->UnloadAllComponents(this);
    
    context.Unbind();
}

//----------------------------------------------------------------------------------------

IWindowRegistry* Application::GetWindowRegistry()
{
    return mWindowManager;
}

//----------------------------------------------------------------------------------------

Wnd::Window* Application::AttachWindow(const AppWindowConfig& appWindowConfig)
{
    PG_ASSERTSTR(mDevice != nullptr, "Application requries render device before attaching window!");
    Alloc::IAllocator* renderAlloc = Memory::GetRenderAllocator();
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Wnd::Window* newWnd = nullptr;
    Wnd::WindowConfig config;

    // Create window
    config.mAllocator = windowAlloc;
    config.mRenderAllocator = renderAlloc;
    config.mDevice = mDevice;
    config.mWindowContext = this;
    config.mIsChild = appWindowConfig.mIsChild;
    config.mParentWindowHandle = appWindowConfig.mParentWindowHandle;
    config.mWidth = appWindowConfig.mWidth;
    config.mHeight = appWindowConfig.mHeight;
    config.mCreateVisible = true;
    
    newWnd = mWindowManager->CreateNewWindow(appWindowConfig.mWindowType, config);

    if (newWnd != nullptr)
    {
        mComponentFactory->AttachComponentsToWindow(newWnd, appWindowConfig.mComponentFlags);
        PG_LOG('APPL', "Window created");
    }
    else
    {
        PG_FAILSTR("Unable to create window!");
    }

    return newWnd;
}

//----------------------------------------------------------------------------------------

void Application::DetachWindow(Wnd::Window* wnd)
{
    mWindowManager->DestroyWindow(wnd);

    PG_LOG('APPL', "Window destroyed");
}

//----------------------------------------------------------------------------------------

//! Starts up the application.
//! \note Creates the dummy startup window used to initialize the OGL extensions.
void Application::StartupAppInternal()
{
    Alloc::IAllocator* renderAlloc = Memory::GetRenderAllocator();
    Pegasus::Render::DeviceConfig deviceConfig;
    deviceConfig.mModuleHandle = mConfig.mModuleHandle;

    mDevice = Pegasus::Render::IDevice::CreatePlatformDevice(deviceConfig, renderAlloc);
    PG_LOG('APPL', "Startup finished");
}

//----------------------------------------------------------------------------------------

void Application::ShutdownAppInternal()
{
    PG_DELETE(Memory::GetRenderAllocator(), mDevice);    
    mDevice = nullptr;
    PG_LOG('APPL', "Device Destroyed");
}


}   // namespace App
}   // namespace Pegasus
