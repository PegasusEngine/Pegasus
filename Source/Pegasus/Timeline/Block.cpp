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

namespace Pegasus {
namespace Timeline {


Block::Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mBeat(0)
,   mDuration(1)
,   mLane(nullptr)
,   mScriptHelper(nullptr)
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
    if (mScriptHelper != nullptr)
    {
        mScriptHelper->Shutdown(); //if no script is open then this is a NOP
        mAppContext->GetTimeline()->GetScriptTracker()->UnregisterScript(mScriptHelper);
        PG_DELETE(mAllocator, mScriptHelper);

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

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    if (mScriptHelper != nullptr)
    {
        mScriptHelper->CallUpdate(beat, mVmState, mScriptVersion);
    }
}

//----------------------------------------------------------------------------------------

void Block::RenderViaScript(float beat, Wnd::Window* window)
{
    if (mScriptHelper != nullptr)
    {
        mScriptHelper->CallRender(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

bool Block::OpenScript(const char* scriptFileName)
{
    if (mScriptHelper == nullptr)
    {
        mScriptHelper = PG_NEW(mAllocator, -1, "Script Helper", Pegasus::Alloc::PG_MEM_PERM) ScriptHelper(mAllocator, mAppContext);
        mAppContext->GetTimeline()->GetScriptTracker()->RegisterScript(mScriptHelper);

        PG_ASSERT(mVmState == nullptr);
        mVmState = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) BlockScript::BsVmState();
        mVmState->Initialize(mAllocator);

        Application::RenderCollection* userContext = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) Application::RenderCollection(mAllocator, mAppContext);
        mVmState->SetUserContext(userContext);

#if PEGASUS_USE_GRAPH_EVENTS
        //register event listener
        mScriptHelper->SetEventListener(mAppContext->GetTimeline()->GetEventListener());
#endif
        mScriptHelper->InvalidateData();
    }
    else
    {
        Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
        if (userCtx != nullptr)
        {
            userCtx->Clean();
        }
        mVmState->Reset();
    }
    mScriptVersion = -1; //restart and invalidate script version
    return mScriptHelper->OpenScript(scriptFileName);
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mScriptHelper->Shutdown();
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
