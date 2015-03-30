#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

TimelineSource::TimelineSource(Alloc::IAllocator* allocator, const char* name, Io::FileBuffer* fileBuffer)
:    Core::SourceCode(allocator), mRefCount(0)
#if PEGASUS_ENABLE_PROXIES
   ,mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER

    mScriptName[0] = '\0';
    PG_ASSERT(Utils::Strlen(name) < MAX_SCRIPT_NAME);
    Utils::Strcat(mScriptName, name);

    SetSource(fileBuffer->GetBuffer(), fileBuffer->GetFileSize());
}

TimelineSource::~TimelineSource()
{
    GRAPH_EVENT_DESTROY_USER_DATA(&mProxy, "BlockScript", GetEventListener());
}

void TimelineSource::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        PG_DELETE(mAllocator, this);
    }
}

void TimelineSource::Compile()
{
    //the reason why we copy is because we are not guaranteed in blockscript to readd the parent.
    //the locking child mechanism does not work, therefore we keep a safe copy of the current parents
    // and then iterate over compilation
    Utils::Vector<SourceCode*> parentsCopy(mAllocator);
    for (int i = 0; i < mParents.GetSize(); ++i)
    {
        parentsCopy.PushEmpty() = mParents[i];
    }

    for (int i = 0; i < parentsCopy.GetSize(); ++i)
    {
        parentsCopy[i]->InvalidateData();
        parentsCopy[i]->Compile();
    }
}
