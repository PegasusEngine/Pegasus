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

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/BlockProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


Block::Block(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext)
:   mAllocator(allocator),
    mAppContext(appContext),
    mBeat(0),
    mDuration(1),
    mLane(nullptr)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the block
    mProxy = PG_NEW(allocator, -1, "Timeline::Block::mProxy", Pegasus::Alloc::PG_MEM_PERM) BlockProxy(this);

    // Default color
    mColorRed = 128;
    mColorGreen = 128;
    mColorBlue = 128;
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Block::~Block()
{
#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the block
    PG_DELETE(mAllocator, mProxy);
#endif
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
    // No default implementation
}

//----------------------------------------------------------------------------------------

void Block::SetBeat(Beat beat)
{
    mBeat = beat;
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
