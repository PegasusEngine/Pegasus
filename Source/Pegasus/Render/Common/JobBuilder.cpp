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

void GpuJob::SetName(const char* name)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    jobInstance.name = name;
}

const char* GpuJob::GetName() const
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    return jobInstance.name.c_str();
}

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
        parentJobInstance.childrenJobs.push_back(mJobHandle);
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
    jobInstance.srvTables[spaceRegister] = resourceTable;
}

void GpuJob::SetUavTable(unsigned spaceRegister, ResourceTableRef uavTable)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (spaceRegister >= (unsigned)jobInstance.uavTables.size())
    {
        jobInstance.uavTables.resize(spaceRegister + 1, nullptr);
    }
    jobInstance.uavTables[spaceRegister] = uavTable;
}


void ComputeJob::SetConstantBuffer(unsigned registerId, BufferRef buffer)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<ComputeCmdData>(&jobInstance.data))
    {
        if ((unsigned)data->cbuffer.size() <= registerId)
            data->cbuffer.resize(registerId + 1, nullptr);
        data->cbuffer[registerId] = buffer;
    }
}

void ComputeJob::SetDispatchParams(unsigned x, unsigned y, unsigned z)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<ComputeCmdData>(&jobInstance.data))
    {
        data->x = x;
        data->y = y;
        data->z = z;
    }
}

template<class JobType>
JobType GenericNext(JobType& objectA, JobType& other, InternalJobBuilder& builder)
{
    const auto& jobInstance = builder.jobTable[objectA.GetInternalJobHandle()];
    auto& otherJobInstance = builder.jobTable[other.GetInternalJobHandle()];
    otherJobInstance.data = jobInstance.data;
	other.AddDependency(objectA);
    return other;
}

ComputeJob ComputeJob::Next()
{
    PG_VERIFY_JOB_HANDLE;
    ComputeJob other = mParent->CreateComputeJob();
    return GenericNext<ComputeJob>(*this, other, *mParent);
}

void DrawJob::SetConstantBuffer(unsigned registerId, BufferRef buffer)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        if ((unsigned)data->cbuffer.size() <= registerId)
            data->cbuffer.resize(registerId + 1, nullptr);
        data->cbuffer[registerId] = buffer;
    }
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

void DrawJob::SetVertexBuffers(BufferRef buffers[], unsigned int count)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        data->vb.clear();
        data->vb.insert(data->vb.end(), buffers, buffers + count);
    }
}

void DrawJob::SetDrawParams(const NonIndexedParams& args)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        data->drawType = DrawCmdData::NonIndexed;
        data->nonIndexed = args;
    }
}

void DrawJob::SetDrawParams(const IndexedParams& args)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DrawCmdData>(&jobInstance.data))
    {
        data->drawType = DrawCmdData::Indexed;
        data->indexed = args;
    }
}

DrawJob DrawJob::Next()
{
    PG_VERIFY_JOB_HANDLE;
    DrawJob other = mParent->CreateDrawJob();
    return GenericNext<DrawJob>(*this, other, *mParent);
}


void CopyJob::Set(BufferRef src, BufferRef dst)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<CopyCmdData>(&jobInstance.data))
    {
        data->src = &(*src);
        data->dst = &(*dst);
    }
}

void CopyJob::Set(TextureRef src, TextureRef dst)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<CopyCmdData>(&jobInstance.data))
    {
        data->src = &(*src);
        data->dst = &(*dst);
    }
}

CopyJob CopyJob::Next()
{
    PG_VERIFY_JOB_HANDLE;
    CopyJob other = mParent->CreateCopyJob();
    return GenericNext<CopyJob>(*this, other, *mParent);
}

void ClearRenderTargetJob::Set(RenderTargetRef rt, float color[4])
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<ClearRenderTargetCmdData>(&jobInstance.data))
    {
        data->rt = rt;
        memcpy(data->color, color, (size_t)(4.0)*sizeof(float));
    }
}

void DisplayJob::SetPresentable(IDisplayRef display)
{
    PG_VERIFY_JOB_HANDLE;
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (auto* data = std::get_if<DisplayCmdData>(&jobInstance.data))
    {
        data->display = display;
    }
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

JobBuilder::~JobBuilder()
{
    PG_DELETE(mDevice->GetAllocator(), mImpl);
}

RootJob JobBuilder::CreateRootJob()
{
    return mImpl->CreateRootJob();
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

ClearRenderTargetJob JobBuilder::CreateClearRenderTargetJob()
{
    return mImpl->CreateClearRenderTargetJob();
}

DisplayJob JobBuilder::CreateDisplayJob()
{
    return mImpl->CreateDisplayJob();
}

GroupJob JobBuilder::CreateGroupJob()
{
    return mImpl->CreateGroupJob();
}

void JobBuilder::Delete(RootJob rootJob)
{
    mImpl->Delete(rootJob);
}

}
}
