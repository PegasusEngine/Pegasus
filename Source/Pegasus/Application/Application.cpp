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
#include "Pegasus/Application/Components/EditorComponents.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Render/IDevice.h"
#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/RenderSystems/System/RenderSystem.h"
#include "Pegasus/PegasusAssetTypes.h"

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

    // Set up debugging facilities
#if PEGASUS_ENABLE_LOG
    Core::LogManager::CreateInstance(coreAlloc);
    Core::LogManager::GetInstance()->RegisterHandler(config.mLoghandler);
#endif
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::CreateInstance(coreAlloc);
    Core::AssertionManager::GetInstance()->RegisterHandler(config.mAssertHandler);
#endif

    Alloc::IAllocator* renderAlloc = Memory::GetRenderAllocator();
    Pegasus::Render::DeviceConfig deviceConfig;
    Pegasus::Render::ContextConfig renderContextConfig;
    deviceConfig.mModuleHandle = mConfig.mModuleHandle;

    mDevice = Pegasus::Render::IDevice::CreatePlatformDevice(deviceConfig, renderAlloc);

    renderContextConfig.mAllocator = Memory::GetCoreAllocator();
    renderContextConfig.mDevice = mDevice;

    mRenderContext = PG_NEW(renderAlloc, -1, "AppRenderContext", Alloc::PG_MEM_PERM) Render::Context(renderContextConfig);

    //Initialize internal state of render library.
    Pegasus::Render::CleanInternalState();

    // Set up the hierarchy of property grid class metadata gathered at compile time
    PropertyGrid::PropertyGridManager::GetInstance().ResolveInternalClassHierarchy();
    mPropertyGridManager = &PropertyGrid::PropertyGridManager::GetInstance();

    // Set up the time system
    Core::InitializePegasusTime();


    // Set up window manager
    mWindowManager = PG_NEW(windowAlloc, -1, "AppWindowManager", Alloc::PG_MEM_PERM) AppWindowManager(windowAlloc);
    
    // Set up node managers
    mNodeManager = PG_NEW(nodeAlloc, -1, "NodeManager", Alloc::PG_MEM_PERM) Graph::NodeManager(nodeAlloc, nodeDataAlloc);

#if PEGASUS_ENABLE_RENDER_API
    Pegasus::Shader::IShaderFactory * shaderFactory = Pegasus::Render::GetRenderShaderFactory();
    Pegasus::Mesh::IMeshFactory * meshFactory = Pegasus::Render::GetRenderMeshFactory();
    Pegasus::Texture::ITextureFactory * textureFactory = Pegasus::Render::GetRenderTextureFactory();

    //! TODO - we probably need to use a render specific allocator for this
    shaderFactory->Initialize(nodeDataAlloc);
    meshFactory->Initialize(nodeDataAlloc);
    textureFactory->Initialize(nodeDataAlloc);
#else
    Pegasus::Shader::IShaderFactory * shaderFactory = nullptr;
    Pegasus::Mesh::IMeshFactory * meshFactory = nullptr;
    Pegasus::Texture::ITextureFactory * textureFactory = nullptr;

#endif

    
    // Set up asset library
    mAssetLib = PG_NEW(nodeAlloc, -1, "AssetLib", Alloc::PG_MEM_PERM) AssetLib::AssetLib(nodeAlloc, nullptr);

    mShaderManager = PG_NEW(nodeAlloc, -1, "ShaderManager", Alloc::PG_MEM_PERM) Shader::ShaderManager(mNodeManager, shaderFactory);
    mTextureManager = PG_NEW(nodeAlloc, -1, "TextureManager", Alloc::PG_MEM_PERM) Texture::TextureManager(mNodeManager, textureFactory);
    mMeshManager = PG_NEW(nodeAlloc, -1, "MeshManager", Alloc::PG_MEM_PERM) Mesh::MeshManager(mNodeManager, meshFactory);
    mBlockScriptManager = PG_NEW(timelineAlloc, -1, "BlockScript Manager", Alloc::PG_MEM_PERM) BlockScript::BlockScriptManager(timelineAlloc);
    mTimelineManager = PG_NEW(timelineAlloc, -1, "Timeline Manager", Alloc::PG_MEM_PERM) Timeline::TimelineManager(timelineAlloc, this);

    mAssetLib->RegisterObjectFactory(mShaderManager);
    mAssetLib->RegisterObjectFactory(mTimelineManager);
    mAssetLib->RegisterObjectFactory(mTextureManager);
    mAssetLib->RegisterObjectFactory(mMeshManager);

    mRenderCollectionFactory = PG_NEW(nodeAlloc, -1, "RenderCollectionFactory", Alloc::PG_MEM_PERM) Pegasus::Application::RenderCollectionFactory(this, nodeAlloc);
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    mBsReflectionInfo = PG_NEW(nodeAlloc, -1, "Bs Reflection Info", Alloc::PG_MEM_PERM) App::AppBsReflectionInfo(nodeAlloc);
#endif

#if PEGASUS_ENABLE_RENDER_API
    //register shader manager into factory, so factory can handle includes
    shaderFactory->RegisterShaderManager(mShaderManager);
#endif

    // Register the entire render api
    mRenderApiScript = mBlockScriptManager->CreateBlockLib("RenderApi");
    Pegasus::Application::RegisterRenderApi(mRenderApiScript, this);

#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    mBsReflectionInfo->RegisterLib(mBlockScriptManager->GetRuntimeLib());
    mBsReflectionInfo->RegisterLib(mRenderApiScript);
#endif
    
    // Cache config
    mConfig = config;

    Wnd::Window* startupWindow = nullptr;

    // Set up IO manager
    // This must be done here because of the GetAppName virtual
    char rootPath[Io::IOManager::MAX_FILEPATH_LENGTH];
    sprintf_s(rootPath, Io::IOManager::MAX_FILEPATH_LENGTH - 1, "%s\\Imported\\", mConfig.mBasePath); // Hardcode imported for now
    mIoManager = PG_NEW(coreAlloc, -1, "IOManager", Pegasus::Alloc::PG_MEM_PERM) Io::IOManager(rootPath);
    
    mAssetLib->SetIoManager(mIoManager); //TODO: decide here if we use the pakIoManager or the standard file system IOManager
    
    mRenderSystemManager = PG_NEW(coreAlloc, -1, "Render System Manager", Alloc::PG_MEM_PERM) RenderSystems::RenderSystemManager(coreAlloc, this);

    PG_LOG('APPL', "Startup finished");
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Alloc::IAllocator* nodeAlloc = Memory::GetNodeAllocator();
    Alloc::IAllocator* nodeDataAlloc = Memory::GetNodeDataAllocator();
    Alloc::IAllocator* timelineAlloc = Memory::GetTimelineAllocator();
    Alloc::IAllocator* coreAlloc  = Memory::GetCoreAllocator();
    Alloc::IAllocator* renderAlloc = Memory::GetRenderAllocator();

    // bind context in case we require deletion of a graphics api resource.
    mRenderContext->Bind();

    PG_DELETE(windowAlloc, mWindowManager);

    // Delete the texture and node managers
    // WARNING: order of destructors is very important.
    PG_DELETE(timelineAlloc, mTimelineManager);
    PG_DELETE(nodeAlloc, mAssetLib);    
    PG_DELETE(nodeAlloc, mMeshManager);
    PG_DELETE(nodeAlloc, mTextureManager);
    PG_DELETE(nodeAlloc, mShaderManager);
    PG_DELETE(nodeAlloc, mNodeManager);
    PG_DELETE(nodeAlloc, mRenderCollectionFactory);
    PG_DELETE(coreAlloc, mRenderSystemManager);

    mBlockScriptManager->DestroyBlockLib(mRenderApiScript);
    PG_DELETE(timelineAlloc, mBlockScriptManager);
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    PG_DELETE(nodeAlloc, mBsReflectionInfo);
#endif
    PG_DELETE(coreAlloc, mIoManager);
    
    //Kill device and context
    PG_DELETE(renderAlloc, mRenderContext);
    mRenderContext = nullptr;
    PG_DELETE(renderAlloc, mDevice);
    mDevice = nullptr;
    PG_LOG('APPL', "Device Destroyed");

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

void Application::Load()
{
    //Load must be called / should be able to be called, before attaching any window.
    //Therefore, any timeline item doing a load time render pass to anything but the default render target
    // (since the default render target belongs to the window) should use the main application context
    mRenderContext->Bind();


    mRenderSystemManager->AddInternalSystems();
    RegisterCustomRenderSystems(mRenderSystemManager);
    
    //register per system shader constants to shader factory
    Pegasus::Shader::IShaderFactory * shaderFactory = Pegasus::Render::GetRenderShaderFactory();
    for (unsigned int i = 0; i < mRenderSystemManager->GetSystemCount(); ++i)
    {
        Utils::Vector<RenderSystems::RenderSystem::ShaderGlobalConstantDesc> descs;
        mRenderSystemManager->GetSystem(i)->OnRegisterShaderGlobalConstants(descs);
        for (unsigned int j = 0; j < descs.GetSize(); ++j)
        {
            RenderSystems::RenderSystem::ShaderGlobalConstantDesc& desc = descs[j];
#if PEGASUS_ENABLE_RENDER_API
            Render::RegisterGlobalConstant(desc.constantName, desc.buffer);
#endif
        }
    }

    mRenderSystemManager->InitializeSystems(this);
    if (mRenderSystemManager->GetLibs().GetSize() != 0)
    {
        mTimelineManager->RegisterExtraLibs(mRenderSystemManager->GetLibs());
    }


    mTimelineManager->GetLibs().PushEmpty() = mRenderApiScript;
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    for (unsigned int i = 0; i < mRenderSystemManager->GetLibs().GetSize(); ++i)
    {
        mBsReflectionInfo->RegisterLib(mRenderSystemManager->GetLibs()[i]);
    }
    //register the timeline lib
    mBsReflectionInfo->RegisterLib(mTimelineManager->GetTimelineLib());
#endif

    
    //! Call custom initialize here
    InitializeApp();

    // Initialize all the components for all the windows.
    mWindowManager->LoadAllComponents(this);

}

//----------------------------------------------------------------------------------------

void Application::Update()
{
    // Needed for compute/etc
    mRenderContext->Bind();

    //! update all components, globally for all the windows.
    mWindowManager->UpdateAllComponents(this);
}

//----------------------------------------------------------------------------------------

void Application::Unload()
{
    mRenderContext->Bind();

    // Initialize all the components for all the windows.
    mWindowManager->UnloadAllComponents(this);
    
#if PEGASUS_ENABLE_RENDER_API
    Render::ClearGlobalConstants();
#endif

    // Custom shutdown, done in the user application
    ShutdownApp();

    // Destroy all the systems this app might have
    mRenderSystemManager->DestroyAllSystems();
}

//----------------------------------------------------------------------------------------

Wnd::Window* Application::AttachWindow(const AppWindowConfig& appWindowConfig)
{
    PG_ASSERTSTR(mDevice != nullptr, "Application requries render device before attaching window!");
    Alloc::IAllocator* renderAlloc = Memory::GetRenderAllocator();
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Wnd::Window* newWnd = nullptr;
    Wnd::WindowConfig config;

    //TODO: simplify this config struct. Too much stuff copied around.
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
    
#if PEGASUS_ENABLE_PROXIES
    config.mRedrawCallbackArg = appWindowConfig.mRedrawArgCallback;
    config.mRedrawEditorCallback = appWindowConfig.mRedrawEditorCallback;
#endif
    newWnd = mWindowManager->CreateNewWindow(config, appWindowConfig.mComponentFlags);

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

}   // namespace App
}   // namespace Pegasus
