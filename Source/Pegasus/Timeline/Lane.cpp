/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Lane.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline lane management, manages a set of blocks on one lane of the timeline

#include "Pegasus/Timeline/Lane.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/LaneProxy.h"
#include <string.h>
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


Lane::Lane(Alloc::IAllocator * allocator)
:   mAllocator(allocator)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Lane object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the lane
    mProxy = PG_NEW(allocator, -1, "Timeline::Lane::mProxy", Pegasus::Alloc::PG_MEM_PERM) LaneProxy(this);

    mName[0] = '\0';
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Lane::~Lane()
{
#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the lane
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void Lane::SetName(const char * name)
{
    if (name == nullptr)
    {
        PG_LOG('TMLN', "Erasing the name of the lane \"%s\"", mName);
        mName[0] = '\0';
    }
    else
    {
        PG_LOG('TMLN', "Setting the name of the lane \"%s\" to \"%s\"", mName, name);
#if PEGASUS_COMPILER_MSVC
        strncpy_s(mName, MAX_NAME_LENGTH, name, MAX_NAME_LENGTH);
#else
        mName[MAX_NAME_LENGTH - 1] = '\0';
        strncpy(mName, name, MAX_NAME_LENGTH - 1);
#endif  // PEGASUS_COMPILER_MSVC
    }

}
#endif  // PEGASUS_ENABLE_PROXIES


}   // namespace Timeline
}   // namespace Pegasus
