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
#include "Pegasus/Application/ScriptRenderApi.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/AppWindowManager.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Render/IDevice.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/TimelineBlock/TimelineBlockRegistration.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Sound/Sound.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include <stdio.h>

namespace Pegasus {
namespace App {

//----------------------------------------------------------------------------------------

Application::Application(const ApplicationConfig& config)
    : mInitialized(false)
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


    mShaderManager = PG_NEW(nodeAlloc, -1, "ShaderManager", Alloc::PG_MEM_PERM) Shader::ShaderManager(mNodeManager, shaderFactory);
    mTextureManager = PG_NEW(nodeAlloc, -1, "TextureManager", Alloc::PG_MEM_PERM) Texture::TextureManager(mNodeManager, textureFactory);
    mMeshManager    = PG_NEW(nodeAlloc, -1, "MeshManager", Alloc::PG_MEM_PERM) Mesh::MeshManager(mNodeManager, meshFactory);

    // Set up blockscript. Use the same timeline allocator
    mBlockScriptManager = PG_NEW(timelineAlloc, -1, "BlockScript Manager", Alloc::PG_MEM_PERM) BlockScript::BlockScriptManager(timelineAlloc);

    // Set up timeline, block script manager has to be initialized first so timeline can push the graphics library within.
    mTimeline = PG_NEW(timelineAlloc, -1, "Timeline", Alloc::PG_MEM_PERM) Timeline::Timeline(timelineAlloc, this);

    // Set up asset library
    mAssetLib = PG_NEW(nodeAlloc, -1, "AssetLib", Alloc::PG_MEM_PERM) AssetLib::AssetLib(nodeAlloc, nullptr);

    // register the entire render api
    Pegasus::Application::RegisterRenderApi(mBlockScriptManager->GetRuntimeLib());

    // Cache config
    mConfig = config;
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Alloc::IAllocator* nodeAlloc = Memory::GetNodeAllocator();
    Alloc::IAllocator* nodeDataAlloc = Memory::GetNodeDataAllocator();
    Alloc::IAllocator* timelineAlloc = Memory::GetTimelineAllocator();

    // Sanity check
    PG_ASSERTSTR(!mInitialized, "Application still initialized in destructor!");

    PG_DELETE(windowAlloc, mWindowManager);

    // Delete the timeline
    PG_DELETE(timelineAlloc, mTimeline);

    // Delete the texture and node managers
    PG_DELETE(nodeAlloc, mMeshManager);
    PG_DELETE(nodeAlloc, mTextureManager);
    PG_DELETE(nodeAlloc, mShaderManager);
    PG_DELETE(nodeAlloc, mNodeManager);

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
    Alloc::IAllocator* coreAlloc = Memory::GetCoreAllocator();
    Wnd::Window* startupWindow = nullptr;

    // Sanity check
    PG_ASSERTSTR(!mInitialized, "Application already initialized!");

    // Set up IO manager
    // This must be done here because of the GetAppName virtual
    char rootPath[Io::IOManager::MAX_FILEPATH_LENGTH];
    sprintf_s(rootPath, Io::IOManager::MAX_FILEPATH_LENGTH - 1, "%s%s\\Imported\\", mConfig.mBasePath, GetAppName()); // Hardcode imported for now
    mIoManager = PG_NEW(coreAlloc, -1, "IOManager", Pegasus::Alloc::PG_MEM_PERM) Io::IOManager(rootPath);

    //TODO: decide here if we use the pakIoManager or the standard file system IOManager
    mAssetLib->SetIoManager(mIoManager);


    // Start up the app, which creates and destroys the dummy window
    StartupAppInternal();

    // Register the Pegasus-side timeline blocks
    TimelineBlock::RegisterBaseBlocks(GetTimeline());

    // Custom initialization, done in the user application
    RegisterTimelineBlocks();
    InitializeApp();

    // Initialized
    mInitialized = true;
}

//----------------------------------------------------------------------------------------

//! Shutdown this application.
void Application::Shutdown()
{
    Alloc::IAllocator* coreAlloc = Memory::GetCoreAllocator();

    // Sanity check
    PG_ASSERTSTR(mInitialized, "Application not initialized yet!");

    // Custom shutdown, done in the user application
    ShutdownApp();

    // Shuts down GLextensions, etc
    ShutdownAppInternal();

    // Tear down IO manager
    PG_DELETE(coreAlloc, mIoManager);

    // No longer initialized
    mInitialized = false;
}

//----------------------------------------------------------------------------------------

void Application::Load()
{
    PG_ASSERTSTR(mTimeline != nullptr, "Invalid timeline for the application");

    // Tell all the blocks of the timeline to initialize their content
    mTimeline->InitializeBlocks();
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

    Sound::Initialize();
}

//----------------------------------------------------------------------------------------

void Application::ShutdownAppInternal()
{
    Sound::Release();

    PG_DELETE(Memory::GetRenderAllocator(), mDevice);    
    mDevice = nullptr;
    PG_LOG('APPL', "Device Destroyed");
}


}   // namespace App
}   // namespace Pegasus
