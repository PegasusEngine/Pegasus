/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   JobBuilder.cpp
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Dx12/Vulcan job based API for GPU rendering

#include <Pegasus/Render/Render.h>
#include <Pegasus/Render/JobBuilder.h>
#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Allocator/IAllocator.h>
#include "InternalJobBuilder.h"
#include <queue>

namespace Pegasus
{
namespace Render
{

void GpuJob::SetGpuPipeline(GpuPipelineRef gpuPipeline)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    jobInstance.pso = gpuPipeline;
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

void GpuJob::SetResourceTable(unsigned spaceRegister, ResourceTableRef resourceTable)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (spaceRegister >= (unsigned)jobInstance.srvTables.size())
    {
        jobInstance.srvTables.resize(spaceRegister + 1, nullptr);
    }
    
}

void ComputeJob::SetUavTable(unsigned spaceRegister, ResourceTableRef uavTable)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<ComputeCmdData>(&jobInstance.data))
    {
        if (spaceRegister >= (unsigned)data->uavTables.size())
        {
            data->uavTables.resize(spaceRegister + 1, nullptr);
        }
        data->uavTables[spaceRegister] = uavTable;
    }
}

void ComputeJob::Dispatch(unsigned x, unsigned y, unsigned z)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
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

void DrawJob::Draw()
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
}

JobBuilder::JobBuilder(IDevice* device)
: mDevice(device)
{
    mImpl = PG_NEW(device->GetAllocator(), -1, "JobBuilderImpl", Pegasus::Alloc::PG_MEM_PERM)
        InternalJobBuilder(device);
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
