/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.cpp
//! \author	Kevin Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#include "Pegasus/Timeline/Block.h"

namespace Pegasus {
namespace Timeline {


Block::Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mBeat(0)
,   mDuration(1)
,   mLane(nullptr)
,   mScriptHelper(allocator, appContext)
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
    mScriptHelper.Shutdown(); //if no script is open then this is a NOP
}

//----------------------------------------------------------------------------------------

void Block::SetBeat(Beat beat)
{
    mBeat = beat;
}

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    mScriptHelper.CallUpdate(beat);
}

//----------------------------------------------------------------------------------------

void Block::RenderViaScript(float beat, Wnd::Window* window)
{
    mScriptHelper.CallRender(beat);
}

//----------------------------------------------------------------------------------------

bool Block::OpenScript(const char* scriptFileName)
{
    return mScriptHelper.OpenScript(scriptFileName);
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    mScriptHelper.Shutdown();
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
