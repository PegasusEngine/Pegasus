/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WorldComponent.cpp
//! \author Kleber Garcia
//! \date   9/10/2015
//! \brief  Main component for world elements - the actual demoscene.

#include "Pegasus/Application/Components/WorldComponent.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Sound/Sound.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/RenderSystems/Camera/CameraSystem.h"
#include "Pegasus/Math/Constants.h"

namespace Pegasus
{
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    namespace Camera
    {
        extern CameraSystem* gCameraSystem;
    }
#endif
}

using namespace Pegasus;
using namespace Pegasus::App;
using namespace Pegasus::Wnd;
using namespace Pegasus::Camera;

BEGIN_IMPLEMENT_PROPERTIES(WorldComponentState)
    IMPLEMENT_PROPERTY(WorldComponentState, IsWireframe)
    IMPLEMENT_PROPERTY(WorldComponentState, EnableFreeCam)
    IMPLEMENT_PROPERTY(WorldComponentState, ResetFreeCam)
END_IMPLEMENT_PROPERTIES(WorldComponentState)

WorldComponentState::WorldComponentState(Alloc::IAllocator* allocator)
: mAllocator(allocator)
{
    BEGIN_INIT_PROPERTIES(WorldComponentState)
        INIT_PROPERTY(IsWireframe)
        INIT_PROPERTY(EnableFreeCam)
        INIT_PROPERTY(ResetFreeCam)
    END_INIT_PROPERTIES();
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    mFreeCam = PG_NEW(allocator, -1, "Free cam", Pegasus::Alloc::PG_MEM_PERM) Camera::Camera(allocator);
    ResetFreeCamPos();
#endif
}
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
void WorldComponentState::ResetFreeCamPos() {
    // initialize math of camera system:
    mFreeCam->SetEnableTarget(1);
    mFreeCam->SetTarget(Math::Vec3(0.0f,0.0f,0.0f));
    mFreeCam->SetTargetDistance(16.0f);
    mFreeCam->SetTargetViewRotation(Math::Vec3(Math::P_PI_OVER_2, -0.5f*Math::P_PI_OVER_4, 0.0f));
    mFreeCam->SetFov(1.2f);
    mFreeCam->Update();
    mFreeCam->SetFar(2500.0f);

#if PEGASUS_ENABLE_PROXIES
    mFreeCam->SetShowCameraOnDebugMode(false); //do not display cam on debug mode.
#endif
}
#endif

WorldComponent::WorldComponent(Alloc::IAllocator *allocator)
   : mAlloc(allocator) {
    Sound::Initialize();
}

WorldComponent::~WorldComponent() {
    Sound::Release();
}

WindowComponentState* WorldComponent::CreateState(const ComponentContext& context) {
    return PG_NEW(mAlloc, -1, "WorldComponent", Pegasus::Alloc::PG_MEM_PERM) WorldComponentState(mAlloc); //no state per window.
}

void WorldComponent::DestroyState(const ComponentContext& context, WindowComponentState *state) {
    PG_DELETE(mAlloc, state);
}

void WorldComponent::Load(Core::IApplicationContext *appContext) {
    PG_ASSERTSTR(appContext->GetTimelineManager() != nullptr, "Invalid timeline for the application");

    Pegasus::Render::RasterizerConfig rasterConfig;
    rasterConfig.mDepthFunc = Pegasus::Render::RasterizerConfig::NONE_DF;
    rasterConfig.mCullMode = Pegasus::Render::RasterizerConfig::CW_CM;
    mDefaultRasterState = Pegasus::Render::CreateRasterizerState(rasterConfig);
    appContext->GetTimelineManager()->InitializeAllTimelines();
}

void WorldComponent::Update(Core::IApplicationContext *appContext) {
    //! \todo Update the sound system once per frame, not per window. Same thing for the timeline above.
    if (!Sound::IsPlayingMusic()) {
        Sound::PlayMusic();
    }
    Sound::Update();

    const unsigned int musicPosition = Sound::GetMusicPosition();
    appContext->GetTimelineManager()->Update(musicPosition);

}

void WorldComponent::WindowUpdate(const ComponentContext& context, Wnd::WindowComponentState *state)
{
}

void WorldComponent::Render(const ComponentContext& context, Wnd::WindowComponentState *state) {
    // Set up rendering
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    context.mTargetWindow->GetDimensions(viewportWidth, viewportHeight);

    WorldComponentState *worldState = static_cast<WorldComponentState *>(state);
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    if (worldState->GetEnableFreeCam()) {
        gCameraSystem->SetFreeCam(worldState->GetFreeCam());
    } else {
        gCameraSystem->SetFreeCam(nullptr);
    }


    if (worldState->GetResetFreeCam())
    {
        worldState->SetResetFreeCam(false);
        worldState->ResetFreeCamPos();
    }

    gCameraSystem->WindowUpdate(context.mTargetWindow);
#endif

    // set default render target
    Pegasus::Render::SetViewport(Pegasus::Render::Viewport(viewportWidth, viewportHeight));
    Pegasus::Render::DispatchDefaultRenderTarget();
    Pegasus::Render::SetRasterizerState(mDefaultRasterState);

    // set clear color and depth
    Pegasus::Render::SetClearColorValue(Pegasus::Math::ColorRGBA(0.0, 0.0, 0.0, 1.0));

    // - todo inverted depth
    Pegasus::Render::SetDepthClearValue(1.0);
    
    // clear buffers
    Pegasus::Render::Clear(/*color*/true, /*depth*/ true, /*stencil*/false);

    // Render the content of the timeline
    Pegasus::Timeline::Timeline* timeline = context.mAppContext->GetTimelineManager()->GetCurrentTimeline();
    if (timeline != nullptr)
    {
        if (static_cast<WorldComponentState*>(state)->GetIsWireframe())
        {
            Pegasus::Render::SetPrimitiveMode(Pegasus::Render::PRIMITIVE_LINE_STRIP);
        }
        else
        {
            Pegasus::Render::SetPrimitiveMode(Pegasus::Render::PRIMITIVE_AUTOMATIC);
        }
        timeline->Render(context.mTargetWindow);
        Pegasus::Render::SetPrimitiveMode(Pegasus::Render::PRIMITIVE_AUTOMATIC);
    }
}

void WorldComponent::Unload(Core::IApplicationContext* appContext)
{
    PG_ASSERTSTR(appContext->GetTimelineManager() != nullptr, "Invalid timeline for the application");
    appContext->GetTimelineManager()->ShutdownAllTimelines();
}

#if PEGASUS_ENABLE_PROXIES
void WorldComponent::OnMouseEvent(Pegasus::Wnd::WindowComponentState* state, Pegasus::Wnd::IWindowComponent::MouseButton button, bool isDown, float x, float y)
{
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    WorldComponentState* componentState = static_cast<WorldComponentState*>(state);
    if (button == IWindowComponent::MouseButton_Cancel)
    {        
        mFreeCamState.cmd = CMD_NONE;
    }
    else
    {
        if (mFreeCamState.cmd == CMD_PAN)
        {
            Camera::Camera* cam = &(*componentState->GetFreeCam());
            cam->ForceUpdate();
            Camera::Camera::GpuCamData& d = cam->GetGpuData();
            Math::Mat44 invViewProj;
            Math::Inverse(invViewProj,d.viewProj);
            Math::Vec4 worldVecA;
            Math::Mult44_41(worldVecA, invViewProj, Math::Vec4(-x,y,0.0f,1.0));
            Math::Vec4 worldVecB;
            Math::Mult44_41(worldVecB, invViewProj, Math::Vec4( -mFreeCamState.x, mFreeCamState.y,0.0f,1.0));
            worldVecA = worldVecA / worldVecA.w;
            worldVecB = worldVecB / worldVecB.w;
            cam->SetTarget(cam->GetTarget() + 4.0f*cam->GetTargetDistance()*Math::Vec3(worldVecA.x - worldVecB.x, worldVecA.y - worldVecB.y, worldVecA.z - worldVecB.z));
            cam->Update();
        }
        else if (mFreeCamState.cmd == CMD_POS_ZOOM || mFreeCamState.cmd == CMD_FOV_ZOOM)
        {
            float zoomDiff =(mFreeCamState.y - y);
            Camera::Camera* cam = &(*componentState->GetFreeCam());
            if (mFreeCamState.cmd == CMD_POS_ZOOM)
            {
                float newDistance = cam->GetTargetDistance() +  0.5f*zoomDiff*(cam->GetTargetDistance());
                newDistance = newDistance < 0.0f ? 0.0f : newDistance;
                cam->SetTargetDistance(newDistance);
            }
            else
            {
                float newFov = cam->GetFov() + 0.5f * zoomDiff;
                cam->SetFov(newFov);
            }
            cam->Update();
        }
        else if (mFreeCamState.cmd == CMD_ORBIT)
        {
            float xDiff = mFreeCamState.x - x;
            float yDiff = mFreeCamState.y - y;
            Camera::Camera* cam = &(*componentState->GetFreeCam());
            Math::Vec3 rotation = cam->GetTargetViewRotation();
            rotation += Math::Vec3(xDiff,yDiff,0.0f);
            //rotation.y = Math::Clamp(rotation.y, -Math::P_PI_OVER_2+0.011f, Math::P_PI_OVER_2-0.011f);
            cam->SetTargetViewRotation(rotation);
            cam->Update();
        }        


        //parse mouse buttons
        if (button == IWindowComponent::MouseButton_Center || mFreeCamState.mKeyStates[Pegasus::Wnd::KEYS_SHIFT] && (button == IWindowComponent::MouseButton_Left))
        {
            mFreeCamState.cmd = isDown ? CMD_PAN : CMD_NONE;
        }
        else if (button == IWindowComponent::MouseButton_Right && !mFreeCamState.mKeyStates[Pegasus::Wnd::KEYS_SHIFT])
        {
            mFreeCamState.cmd = isDown ? CMD_POS_ZOOM : CMD_NONE;
        }
        else if (button == IWindowComponent::MouseButton_Right && mFreeCamState.mKeyStates[Pegasus::Wnd::KEYS_SHIFT])
        {
            mFreeCamState.cmd = isDown ? CMD_FOV_ZOOM : CMD_NONE;
        }
        else if (button == IWindowComponent::MouseButton_Left)
        {
            mFreeCamState.cmd = isDown ? CMD_ORBIT : CMD_NONE;
        }


        //refresh mouse data.
        mFreeCamState.x = x;
        mFreeCamState.y = y;
    }
#endif
}

void WorldComponent::OnKeyEvent(Wnd::WindowComponentState* state, Pegasus::Wnd::Keys key, bool isDown)
{
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    mFreeCamState.mKeyStates[key] = isDown;
#endif
}
#endif
