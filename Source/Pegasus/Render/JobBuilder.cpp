#include <Pegasus/Render/Render.h>
#include <vector>
#include <variant>
#include <map>
#include <set>

namespace Pegasus
{
namespace Render
{

struct ComputeCmdData
{
    std::vector<ResourceTableRef> uavTables;
};

struct DrawCmdData
{
    RenderTargetRef rt;
};

typedef std::variant<DrawCmdData, ComputeCmdData> VariantData;

struct JobInstance
{
    InternalJobHandle handle;

    std::set<InternalJobHandle> dependenciesSet;
    std::vector<InternalJobHandle> dependenciesSorted;
    std::vector<InternalJobHandle> childrenJobs;

    GpuPipelineRef pso;
    std::vector<ResourceTableRef> resTables;
    VariantData data;
};

class ResourceStates
{
public:
    ResourceStateId AddRootResource(IResourceRef res);

private:
    struct StateContainer
    {
        IResourceRef resource;
    };
    std::vector<StateContainer> mResStateMap;
};

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

class InternalJobBuilder
{
public:

    ComputeJob CreateComputeJob()
    {
        JobInstance newInstance;
        auto outJob = NewXJob<ComputeJob>(newInstance);
        newInstance.data = ComputeCmdData();
        return outJob;
    }

    DrawJob CreateDrawJob()
    {
        JobInstance newInstance;
        auto outJob = NewXJob<DrawJob>(newInstance);
        newInstance.data = DrawCmdData();
        return outJob;
    }

    ResourceStateId Import(IResourceRef resourceRef);

    std::vector<JobInstance> jobTable;

    ResourceStates mResStates;

private:

	template<class JobType>
    JobType NewXJob(JobInstance& outInstance)
    {
        InternalJobHandle newHandle = InvalidJobHandle;
        
        if (mFreeJobs.empty())
        {
            newHandle = (InternalJobHandle)jobTable.size();
            jobTable.emplace_back();
        }
        else
        {
            newHandle = mFreeJobs.back();
            mFreeJobs.pop_back();
        }
        outInstance = jobTable[newHandle];
        return JobType(newHandle, this);
    }

    std::vector<InternalJobHandle> mFreeJobs;
};

ResourceStateId InternalJobBuilder::Import(IResourceRef resourceRef)
{
    return mResStates.AddRootResource(resourceRef);
}

void GpuJob::SetGpuPipeline(GpuPipelineRef gpuPipeline)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    jobInstance.pso = gpuPipeline;
}

void GpuJob::SetResourceTable(int registerSpace, ResourceTableRef resourceTable)
{
    PG_ASSERT(mJobHandle < (InternalJobHandle)mParent->jobTable.size());
    auto& jobInstance = mParent->jobTable[mJobHandle];
    if (registerSpace >= (int)jobInstance.resTables.size())
    {
        jobInstance.resTables.resize(registerSpace + 1, nullptr);
    }

    jobInstance.resTables[registerSpace] = resourceTable;
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
        parentJobInstance.childrenJobs.push_back(other.mJobHandle);
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

}
}
