#include "ADevice.h"
#include "JobTreeVisitors.h"

namespace Pegasus
{
namespace Render
{

GpuWorkManager::GpuWorkManager(Pegasus::Alloc::IAllocator* allocator, ResourceStateTable& stateTable)
: mAllocator(allocator), mResourceStateTable(stateTable)
{
}

GpuSubmitResult GpuWorkManager::Submit(const RootJob& rootJob, GpuWorkManager::SubmitGpuWorkFn platformWorkFn)
{
    CanonicalCmdListBuilder cmdListBuilder(mAllocator, mResourceStateTable);
    CanonicalCmdListResult result;
    cmdListBuilder.Build(rootJob, result);

    if (result.staleJobCounts > 0u)
        return GpuSubmitResult{ {}, GpuWorkResultCode::CompileError_StaleJobs };

    if (result.barrierViolationsCount > 0u)
        return GpuSubmitResult{ {}, GpuWorkResultCode::CompileError_StaleJobs };

    return platformWorkFn(result);
}

void GpuWorkManager::ReleaseWork(GpuWorkHandle workHandle)
{
}

bool GpuWorkManager::IsFinished(GpuWorkHandle workHandle)
{
	return false;
}

void GpuWorkManager::GarbageCollect()
{
}

}
}
