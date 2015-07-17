#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

TimelineSource::TimelineSource(Alloc::IAllocator* allocator, const char* name)
:    Core::SourceCode(allocator), mRefCount(0)
#if PEGASUS_ENABLE_PROXIES
   ,mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER

    mScriptName[0] = '\0';
    PG_ASSERT(Utils::Strlen(name) < MAX_SCRIPT_NAME);
    Utils::Strcat(mScriptName, name);
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

bool TimelineSource::IsTimelineScript(const Pegasus::AssetLib::Asset* asset)
{
    const char* path = asset->GetPath();
    const char* extension = Utils::Strrchr(path, '.');
    return asset->GetFormat() == AssetLib::Asset::FMT_RAW && extension != nullptr && 
        (!Utils::Strcmp(".bs", extension) || !Utils::Strcmp(".bsh", extension));
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
