/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.cpp
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/ScriptTracker.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"

namespace Pegasus {
namespace Timeline {


Block::Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mBeat(0)
,   mDuration(1)
,   mLane(nullptr)
,   mTimelineScript(nullptr)
,   mVmState(nullptr)
,   mScriptVersion(-1)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mColorRed(128)
,   mColorGreen(128)
,   mColorBlue(128)
#endif  // PEGASUS_ENABLE_PROXIES
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");
}

//----------------------------------------------------------------------------------------

Block::~Block()
{

}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

void Block::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    mColorRed = red;
    mColorGreen = green;
    mColorBlue = blue;
}

//----------------------------------------------------------------------------------------

void Block::GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const
{
    red = mColorRed;
    green = mColorGreen;
    blue = mColorBlue;
}

#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

void Block::Initialize()
{
    // No default implementation
}

//----------------------------------------------------------------------------------------

void Block::Shutdown()
{
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
        mAppContext->GetTimeline()->GetScriptTracker()->UnregisterScript(mTimelineScript);
        mTimelineScript = nullptr;

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
            PG_DELETE(mAllocator, userCtx);
        }
        PG_DELETE(mAllocator, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::SetBeat(Beat beat)
{
    mBeat = beat;
}

bool Block::InitializeScript()
{
    bool didChangeRuntimeLayout = false;
    
    //TODO: remove this global scope destroy stuff
    if (mTimelineScript->IsDirty())
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
    }

    mTimelineScript->Compile();

    if (mScriptVersion != mTimelineScript->GetSerialVersion())
    {
        mScriptVersion = mTimelineScript->GetSerialVersion();

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
            nodeContaier->Clean();
        }
        mVmState->Reset();

        //re-initialize everything!
        mTimelineScript->CallGlobalScopeInit(mVmState);     

        didChangeRuntimeLayout = didChangeRuntimeLayout || true;
    }
    return didChangeRuntimeLayout;
}

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        if (InitializeScript())// attempt to initialize if script was modified
        {
            TimelineScript* rawptr = &(*mTimelineScript);
            //Throw an event to the UI
            GRAPH_EVENT_DISPATCH(
                rawptr,
                Core::CompilerEvents::CompilationNotification,
                //Event that notifies that the runtime has changed, potentially changing layout of nodes and such.
                // This means that some views will require to be updated
                Core::CompilerEvents::CompilationNotification::COMPILATION_RUNTIME_INITIALIZATION, 0, ""
            );
        }
        mTimelineScript->CallUpdate(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::RenderViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->CallRender(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::AttachScript(TimelineScriptIn script)
{
    if (mTimelineScript == nullptr)
    {
        mTimelineScript = script;

        PG_ASSERT(mVmState == nullptr);
        mVmState = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) BlockScript::BsVmState();
        mVmState->Initialize(mAllocator);

        Application::RenderCollection* userContext = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) Application::RenderCollection(mAllocator, mAppContext);
        mVmState->SetUserContext(userContext);
    }
    else
    {
        mTimelineScript = script;
        Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
        if (userCtx != nullptr)
        {
            userCtx->Clean();
        }
        mVmState->Reset();
    }
    
    mScriptVersion = -1;
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mTimelineScript = nullptr;
}

//----------------------------------------------------------------------------------------

void Block::SetDuration(Duration duration)
{
    if (duration == 0)
    {
        mDuration = 1;
        PG_FAILSTR("Invalid duration given to the block (%u), setting it to 1 tick", duration);
    }
    else
    {
        mDuration = duration;
    }
}

//----------------------------------------------------------------------------------------

void Block::SetLane(Lane * lane)
{
    PG_ASSERTSTR(lane != nullptr, "Invalid lane associated with a block");
    mLane = lane;
}


}   // namespace Timeline
}   // namespace Pegasus
