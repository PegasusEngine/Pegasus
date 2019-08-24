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
    InternalJobHandle handle = InvalidJobHandle;

    std::set<InternalJobHandle> dependenciesSet;
    std::vector<InternalJobHandle> dependenciesSorted;
    std::vector<InternalJobHandle> parentJobs;

    GpuPipelineRef pso;
    VariantData data;
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

    InternalJobHandle CreateJobInstance();

    void SubmitRootJob();

    std::vector<JobInstance> jobTable;

private:

    IDevice* mDevice;
};


}
}
