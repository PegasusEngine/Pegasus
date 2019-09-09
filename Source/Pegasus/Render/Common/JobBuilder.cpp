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

#define PG_VERIFY_JOB_HANDLE PG_ASSERT(mJobHandle >= 0 && mJobHandle < (InternalJobHandle)mParent->jobTable.size())

namespace Pegasus
{
namespace Render
{

void GpuJob::SetGpuPipeline(GpuPipelineRef gpuPipeline)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    jobInstance.pso = gpuPipeline;
}

void GpuJob::AddDependency(const GpuJob& other)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    
    auto it = jobInstance.dependenciesSet.insert(other.mJobHandle);
    if (it.second)
    {
        jobInstance.dependenciesSorted.push_back(other.mJobHandle);
        PG_ASSERT(other.mJobHandle < (InternalJobHandle)mParent->jobTable.size());
        auto& parentJobInstance = mParent->jobTable[other.mJobHandle];
        parentJobInstance.childrenJobs.push_back(other.mJobHandle);
    }
}

void GpuJob::SetResourceTable(unsigned spaceRegister, ResourceTableRef resourceTable)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (spaceRegister >= (unsigned)jobInstance.srvTables.size())
    {
        jobInstance.srvTables.resize(spaceRegister + 1, nullptr);
    }
    
}

void ComputeJob::SetUavTable(unsigned spaceRegister, ResourceTableRef uavTable)
{
    PG_VERIFY_JOB_HANDLE;
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
    PG_VERIFY_JOB_HANDLE;
}

ComputeJob ComputeJob::Next()
{
    PG_VERIFY_JOB_HANDLE;
    const auto& jobInstance = mParent->jobTable[mJobHandle];
    ComputeJob other = mParent->CreateComputeJob();
    auto& otherJobInstance = mParent->jobTable[other.mJobHandle];
    otherJobInstance.data = jobInstance.data;
    AddDependency(other);
    return other;
}

void DrawJob::SetRenderTarget(RenderTargetRef renderTargets)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        data->rt = renderTargets;
    }
}

void DrawJob::Draw()
{
    PG_VERIFY_JOB_HANDLE;
}

DrawJob DrawJob::Next()
{
    PG_VERIFY_JOB_HANDLE;
    const auto& jobInstance = mParent->jobTable[mJobHandle];
    DrawJob other = mParent->CreateDrawJob();
    auto& otherJobInstance = mParent->jobTable[other.mJobHandle];
    otherJobInstance.data = jobInstance.data;
    AddDependency(other);
    return other;
}


void CopyJob::Set(BufferRef src, BufferRef dst)
{
    PG_VERIFY_JOB_HANDLE;
}

void CopyJob::Set(TextureRef src, TextureRef dst)
{
    PG_VERIFY_JOB_HANDLE;
}

void GroupJob::AddJob(const GpuJob& other)
{
    PG_VERIFY_JOB_HANDLE;
}

void GroupJob::AddJobs(const GpuJob* jobs, unsigned count)
{
    PG_VERIFY_JOB_HANDLE;
}

JobBuilder::JobBuilder(IDevice* device)
: mDevice(device)
{
    mImpl = PG_NEW(device->GetAllocator(), -1, "JobBuilderImpl", Pegasus::Alloc::PG_MEM_PERM)
        InternalJobBuilder(device);
}

RootJob JobBuilder::CreateRootJob()
{
    return mImpl->CreateRootJob();
}

bool JobBuilder::Execute(RootJob rootJob)
{
    return mImpl->Execute(rootJob);
}

ComputeJob JobBuilder::CreateComputeJob()
{
    return mImpl->CreateComputeJob();
}

DrawJob JobBuilder::CreateDrawJob()
{
    return mImpl->CreateDrawJob();
}

CopyJob JobBuilder::CreateCopyJob()
{
    return mImpl->CreateCopyJob();
}

DisplayJob JobBuilder::CreateDisplayJob()
{
    return mImpl->CreateDisplayJob();
}

GroupJob JobBuilder::CreateGroupJob()
{
    return mImpl->CreateGroupJob();
}

}
}
