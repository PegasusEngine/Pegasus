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

using namespace Pegasus;
using namespace Pegasus::App;
using namespace Pegasus::Wnd;

WorldComponent::WorldComponent(Alloc::IAllocator* allocator)
: mAlloc(allocator)
{
    Sound::Initialize();
}

WorldComponent::~WorldComponent()
{
    Sound::Release();
}

IWindowComponent::IState* WorldComponent::CreateState(const ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "WorldComponent", Pegasus::Alloc::PG_MEM_PERM) WorldComponentState; //no state per window.
}

void WorldComponent::DestroyState(const ComponentContext& context, IState* state)
{
    PG_DELETE(mAlloc, state);
}

void WorldComponent::Load(Core::IApplicationContext* appContext)
{
    PG_ASSERTSTR(appContext->GetTimelineManager() != nullptr, "Invalid timeline for the application");
    
    Pegasus::Render::RasterizerConfig rasterConfig;
    rasterConfig.mDepthFunc = Pegasus::Render::RasterizerConfig::NONE_DF;
    rasterConfig.mCullMode = Pegasus::Render::RasterizerConfig::CW_CM;
    Pegasus::Render::CreateRasterizerState(rasterConfig, mDefaultRasterState);
    appContext->GetTimelineManager()->InitializeAllTimelines();
}

void WorldComponent::Update(Core::IApplicationContext* appContext)
{
    //! \todo Update the sound system once per frame, not per window. Same thing for the timeline above.
    if (!Sound::IsPlayingMusic())
    {
        Sound::PlayMusic();
    }
    Sound::Update();

    const unsigned int musicPosition = Sound::GetMusicPosition();            
    appContext->GetTimelineManager()->Update(musicPosition);
}

void WorldComponent::WindowUpdate(const ComponentContext& context, IState* state)
{
    /* no update per window per timeline */
}

void WorldComponent::Render(const ComponentContext& context, IState* state)
{
    // Set up rendering
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    context.mTargetWindow->GetDimensions(viewportWidth, viewportHeight);
    
    // set default render target
    Pegasus::Render::SetViewport(Pegasus::Render::Viewport(viewportWidth, viewportHeight));
    Pegasus::Render::DispatchDefaultRenderTarget();
    Pegasus::Render::SetRasterizerState(mDefaultRasterState);

    // set clear color and depth
    Pegasus::Render::SetClearColorValue(Pegasus::Math::ColorRGBA(0.0, 0.0, 0.0, 1.0));

    // - todo inverted depth
    Pegasus::Render::SetDepthClearValue(0.0);
    
    // clear buffers
    Pegasus::Render::Clear(/*color*/true, /*depth*/ true, /*stencil*/false);

    // Render the content of the timeline
    Pegasus::Timeline::Timeline* timeline = context.mAppContext->GetTimelineManager()->GetCurrentTimeline();
    if (timeline != nullptr)
    {
        timeline->Render(context.mTargetWindow);
    }
}

void WorldComponent::Unload(Core::IApplicationContext* appContext)
{
    PG_ASSERTSTR(appContext->GetTimelineManager() != nullptr, "Invalid timeline for the application");
    appContext->GetTimelineManager()->ShutdownAllTimelines();
    Pegasus::Render::DeleteRasterizerState(mDefaultRasterState);
}
