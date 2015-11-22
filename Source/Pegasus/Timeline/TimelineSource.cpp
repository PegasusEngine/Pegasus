#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

TimelineSource::TimelineSource(Alloc::IAllocator* allocator)
:    Core::SourceCode(allocator,allocator)
#if PEGASUS_ENABLE_PROXIES
   ,mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER
}

TimelineSource::~TimelineSource()
{
    GRAPH_EVENT_DESTROY_USER_DATA(&mProxy, "BlockScript", GetEventListener());
}

void TimelineSource::Compile()
{
    //the reason why we copy is because we are not guaranteed in blockscript to readd the parent.
    //the locking child mechanism does not work, therefore we keep a safe copy of the current parents
    // and then iterate over compilation
    Utils::Vector<SourceCode*> parentsCopy(mAllocator);
    for (unsigned int i = 0; i < mParents.GetSize(); ++i)
    {
        parentsCopy.PushEmpty() = mParents[i];
    }

    for (unsigned int i = 0; i < parentsCopy.GetSize(); ++i)
    {
        parentsCopy[i]->InvalidateData();
        parentsCopy[i]->Compile();
    }
}
