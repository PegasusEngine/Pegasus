#include <Pegasus/Render/Render.h>
#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Allocator/IAllocator.h>
#include "JobBuilder.h"
#include <queue>

namespace Pegasus
{
namespace Render
{

ResourceStateId ResourceStates::AddRootResource(IResourceRef res)
{
    StateContainer newContainer;
    newContainer.resource = res;
    ResourceStateId newStateId;
    newStateId.index = (int)mResStateMap.size();
    newStateId.versionId = 0;
    mResStateMap.push_back(newContainer);
    return newStateId;
}

ResourceStateId InternalJobBuilder::Import(IResourceRef resourceRef)
{
    return mResStates.AddRootResource(resourceRef);
}

InternalJobHandle InternalJobBuilder::CreateJobInstance()
{
    InternalJobHandle newHandle = (InternalJobHandle)jobTable.size();
    jobTable.emplace_back();
    jobTable[newHandle].handle = newHandle;
    return newHandle;
}

InternalTableHandle InternalJobBuilder::CreateResourceTable()
{
    InternalTableHandle newHandle = (InternalTableHandle)resourceTables.size();
    resourceTables.emplace_back();
    resourceTables[newHandle].handle = newHandle;
    return newHandle;
}

void InternalJobBuilder::SubmitRootJob()
{
    if (jobTable.empty())
        return;

    struct NodeState
    {
        InternalJobHandle handle;
    };

    std::queue<NodeState> processQueue;
    processQueue.push(NodeState{ 0u });

    while (!processQueue.empty())
    {
        auto& state = processQueue.front();
        processQueue.pop();

        auto& jobInstance = jobTable[state.handle];
        if (state.handle != 0)
        {
            //TODO: do something with hardware queue
        }

        //push children into queue
        for (auto pHandle : jobInstance.parentJobs)
            processQueue.push(NodeState { pHandle });
    }
}

void ResourceTable::SetResource(unsigned resourceRegister, ResourceStateId resourceState)
{
    PG_ASSERT(mHandle < (InternalJobHandle)mParent->resourceTables.size());
    auto& tableInstance = mParent->resourceTables[mHandle];
    if (resourceRegister >= (unsigned)tableInstance.resources.size())
        tableInstance.resources.resize(resourceRegister + 1, InvalidResourceStateId);

    tableInstance.resources[resourceRegister] = resourceState;
}

void GpuJob::SetGpuPipeline(GpuPipelineRef gpuPipeline)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    jobInstance.pso = gpuPipeline;
}

ResourceTable GpuJob::GetResourceTable(unsigned registerSpace)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (registerSpace >= (unsigned)jobInstance.resTables.size())
    {
        jobInstance.resTables.resize(registerSpace + 1, InvalidTableHandle);
    }

    InternalTableHandle tableHandle = jobInstance.resTables[registerSpace];
    if (tableHandle == InvalidTableHandle)
    {
        tableHandle = mParent->CreateResourceTable();
    }

    return ResourceTable(tableHandle, mJobHandle, mParent);
}

void GpuJob::DependsOn(const GpuJob& other)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    
    auto it = jobInstance.dependenciesSet.insert(other.mJobHandle);
    if (it.second)
    {
        jobInstance.dependenciesSorted.push_back(other.mJobHandle);
        PG_ASSERT(other.mJobHandle < (InternalJobHandle)mParent->jobTable.size());
        auto& parentJobInstance = mParent->jobTable[other.mJobHandle];
        parentJobInstance.parentJobs.push_back(other.mJobHandle);
    }
}

void ComputeJob::SetUavTable(int registerSpace, ResourceTableRef uavTable)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<ComputeCmdData>(&jobInstance.data))
    {
        if (registerSpace >= (int)data->uavTables.size())
        {
            data->uavTables.resize(registerSpace + 1, nullptr);
        }
        data->uavTables[registerSpace] = uavTable;
    }
}

JobOutput ComputeJob::Dispatch(unsigned x, unsigned y, unsigned z)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    return JobOutput(mJobHandle, mParent);
}

void DrawJob::SetRenderTarget(RenderTargetRef renderTargets)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        data->rt = renderTargets;
    }
}

JobOutput DrawJob::Draw()
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    return JobOutput(mJobHandle, mParent);
}

ResourceStateId JobOutput::ReadOutputIndex(int idx)
{
    return InvalidResourceStateId;
}

ResourceStateId JobOutput::ReadDepthOuput()
{
    return InvalidResourceStateId;
}

JobBuilder::JobBuilder(IDevice* device)
: mDevice(device)
{
    mImpl = PG_NEW(device->GetAllocator(), -1, "JobBuilderImpl", Pegasus::Alloc::PG_MEM_PERM)
        InternalJobBuilder(device);
}

ResourceStateId JobBuilder::Import(IResourceRef resourceRef)
{
    return mImpl->Import(resourceRef);
}

GpuJob JobBuilder::RootJob()
{
    if (mImpl->jobTable.empty())
        mImpl->jobTable.emplace_back();

    return GpuJob(0u, mImpl);
}

void JobBuilder::SubmitRootJob()
{
    mImpl->SubmitRootJob();
}

ComputeJob JobBuilder::CreateComputeJob()
{
    return mImpl->CreateComputeJob();
}

DrawJob JobBuilder::CreateDrawJob()
{
    return mImpl->CreateDrawJob();
}

}
}
