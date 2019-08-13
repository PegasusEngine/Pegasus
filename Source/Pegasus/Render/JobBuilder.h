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
    InternalJobHandle handle = InvalidJobHandle;

    std::set<InternalJobHandle> dependenciesSet;
    std::vector<InternalJobHandle> dependenciesSorted;
    std::vector<InternalJobHandle> parentJobs;

    GpuPipelineRef pso;
    std::vector<ItnernalTableHandle> resTables;
    VariantData data;
};

struct ResourceTableInstance
{
    InternalTableHandle handle = InvalidTableHandle;
    std::vector<ResourceStateId> resources;
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
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = ComputeCmdData();
        return ComputeJob(h, this);
    }

    DrawJob CreateDrawJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = DrawCmdData();
        return DrawJob(h, this);
    }

    ResourceStateId Import(IResourceRef resourceRef);

    InternalTableHandle CreateResourceTable();
    InternalJobHandle CreateJobInstance();

    void SubmitRootJob();

    std::vector<JobInstance> jobTable;
    std::vector<ResourceTableInstance> resourceTables;

    ResourceStates mResStates;

private:

    IDevice* mDevice;
};


}
}
