/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   InternalJobBuilder.h
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Internal common platform API for job builder

#pragma once

#include <Pegasus/Render/JobBuilder.h>
#include <vector>
#include <string>
#include <variant>
#include "ResourceStateTable.h"

namespace Pegasus
{
namespace Render
{

struct RootCmdData
{
    bool cachedChildren = false;
    std::vector<InternalJobHandle> cachedChildrenJobs;
};

struct DrawCmdData
{
    std::vector<BufferRef> cbuffer;
    RenderTargetRef rt;
};

struct ComputeCmdData
{
    std::vector<BufferRef> cbuffer;
};

struct CopyCmdData
{
    Core::Ref<IResource> src;
    Core::Ref<IResource> dst;
};

struct DisplayCmdData
{
};

struct GroupCmdData
{
};

typedef std::variant<
		RootCmdData,
        DrawCmdData,
        ComputeCmdData,
        CopyCmdData,
        DisplayCmdData,
        GroupCmdData> VariantData;

struct JobInstance
{
    InternalJobHandle handle = InvalidJobHandle;

    std::string name;
    std::vector<ResourceTableRef> srvTables;
    std::vector<ResourceTableRef> uavTables;

    std::set<InternalJobHandle> dependenciesSet;
    std::vector<InternalJobHandle> dependenciesSorted;
    std::vector<InternalJobHandle> childrenJobs;

    GpuPipelineRef pso;
    VariantData data;
};

class InternalJobBuilder
{
public:
    InternalJobBuilder(IDevice* device);
    ~InternalJobBuilder();

    RootJob CreateRootJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = RootCmdData();
        return RootJob(h, this);
    }

    DrawJob CreateDrawJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = DrawCmdData();
        return DrawJob(h, this);
    }

    ComputeJob CreateComputeJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = ComputeCmdData();
        return ComputeJob(h, this);
    }

    CopyJob CreateCopyJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = CopyCmdData();
        return CopyJob(h, this);
    }

    DisplayJob CreateDisplayJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = DisplayCmdData();
        return DisplayJob(h, this);
    }

    GroupJob CreateGroupJob()
    {
        InternalJobHandle h = CreateJobInstance();
        jobTable[h].data = GroupCmdData();
        return GroupJob(h, this);
    }

    void Delete(RootJob rootJob);

    InternalJobHandle CreateJobInstance();

    std::vector<JobInstance> jobTable;

private:
    IDevice* mDevice;
    ResourceStateTable::Domain mStateDomain;
    std::vector<InternalJobHandle> mFreeSlots;
};

}
}
