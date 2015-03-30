/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineSource.h
//! \author	Kleber Garcia
//! \date	March 15 2015
//! \brief	Script base file containing source

#ifndef TIMELINE_SOURCE_H
#define TIMELINE_SOURCE_H

#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Allocator/IAllocator.h"

#define MAX_SCRIPT_NAME 64

namespace Pegasus {
    namespace Io {
        class FileBuffer;
    }
}

namespace Pegasus
{
namespace Timeline
{

class TimelineSource : public Core::SourceCode
{
    template<class C> friend class Pegasus::Core::Ref;

public:
    //! constructor
    TimelineSource(Alloc::IAllocator* allocator, const char* name, Io::FileBuffer* fileBuffer);

    //! destructor
    virtual ~TimelineSource();

    //! Gets the script name
    const char* GetScriptName() const { return mScriptName; }

    virtual void InvalidateData() {}

    virtual void Compile();

#if PEGASUS_ENABLE_PROXIES
    //! Gets the proxy 
    //! \return Proxy to this script
    TimelineScriptProxy* GetProxy() { return &mProxy; }
#endif

private:

    //! Increment the reference counter, used by Ref<Node>
    inline void AddRef() { mRefCount++; }

    //! Decrease the reference counter, and delete the current object
    //! if the counter reaches 0
    void Release();

    //! Reference counter
    int mRefCount;

#if PEGASUS_ENABLE_PROXIES
    TimelineScriptProxy mProxy;
#endif

    //! copy of the script name
    char mScriptName[MAX_SCRIPT_NAME];
};

//! Reference to a Node, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TimelineSource>   TimelineSourceRef;

//! Const reference to a reference to a Node, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TimelineSource> & TimelineSourceIn;

//! Reference to a reference to a Node, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TimelineSource> & TimelineSourceInOut;

//! Reference to a Node, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TimelineSource>   TimelineSourceReturn;

}
}

#endif
