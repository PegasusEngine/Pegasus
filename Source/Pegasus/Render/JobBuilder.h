#pragma once

#include <Pegasus/Render/Render.h>
#include <vector>
#include <variant>
#include <map>
#include <set>

namespace Pegasus
{
namespace Render
{

class IDevice;

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
    std::vector<InternalJobHandle> parentJobs;

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

class InternalJobBuilder
{
public:
    InternalJobBuilder(IDevice* device) : mDevice(device) {}

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

    void SubmitRootJob();

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
    IDevice* mDevice;
};


}
}
