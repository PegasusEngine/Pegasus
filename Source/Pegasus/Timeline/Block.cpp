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


Block::Block(Alloc::IAllocator * allocator)
:   mPosition(0.0f),
    mLength(1.0f)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the block
    mProxy = PG_NEW(allocator, -1, "Timeline::Block::mProxy", Pegasus::Alloc::PG_MEM_PERM) BlockProxy(this);
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

void Block::SetPosition(float position)
{
    PG_LOG('TMLN', "Setting the position of a block to %f", position);

    if (position < 0.0f)
    {
        mPosition = 0.0f;
        PG_FAILSTR("Invalid position given to a block (%f), setting it to 0.0f", position);
    }
    else
    {
        mPosition = position;
    }
}

//----------------------------------------------------------------------------------------

void Block::SetLength(float length)
{
    PG_LOG('TMLN', "Setting the length of a block to %f", length);

    if (length <= 0.0f)
    {
        mLength = 1.0f;
        PG_FAILSTR("Invalid length given to a block (%f), setting it to 1.0f", length);
    }
    else
    {
        mLength = length;
    }
}


}   // namespace Timeline
}   // namespace Pegasus
