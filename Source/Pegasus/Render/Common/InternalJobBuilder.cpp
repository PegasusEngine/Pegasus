/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   InternalJobBuilder.cpp
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Dx12/Vulcan job based API for GPU rendering

#include "InternalJobBuilder.h"
#include <Pegasus/Render/Render.h>
#include <Pegasus/Render/JobBuilder.h>
#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Allocator/IAllocator.h>
#include "JobTreeVisitors.h"
#include "JobTreeParser.h"
#include <queue>

namespace Pegasus
{
namespace Render
{

InternalJobBuilder::InternalJobBuilder(IDevice* device)
: mDevice(device)
{
	mStateDomain = device->GetResourceStateTable()->CreateDomain();
}

InternalJobBuilder::~InternalJobBuilder()
{
    mDevice->GetResourceStateTable()->RemoveDomain(mStateDomain);
}

InternalJobHandle InternalJobBuilder::CreateJobInstance()
{
    InternalJobHandle newHandle;
    if (mFreeSlots.empty())
    {
        newHandle = (InternalJobHandle)jobTable.size();
        jobTable.emplace_back();
        jobTable[newHandle].handle = newHandle;
    }
    else
    {
        newHandle = mFreeSlots.back();
    }

    return newHandle;
}

void InternalJobBuilder::Delete(RootJob rootJob)
{
    InternalJobHandle h = rootJob.GetInternalJobHandle();
    PG_ASSERT(h >= 0 && h < (InternalJobHandle)jobTable.size());
    auto& jobInstance = jobTable[h];
    auto& data = std::get<RootCmdData>(jobInstance.data);

    if (!data.cachedChildren)
    {
        ChildJobAccumulator visitor(data.cachedChildrenJobs);
        ParseRootJobBFS<ChildJobAccumulator>(rootJob, visitor);
        data.cachedChildren = true;
    }

    for (auto& childJobHandle : data.cachedChildrenJobs)
    {
		if (childJobHandle == rootJob.GetInternalJobHandle())
			continue;
        mFreeSlots.push_back(childJobHandle);
        jobTable[childJobHandle] = JobInstance();
    }
}

}
}
