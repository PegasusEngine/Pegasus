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
#include <queue>

namespace Pegasus
{
namespace Render
{

InternalJobHandle InternalJobBuilder::CreateJobInstance()
{
    InternalJobHandle newHandle = (InternalJobHandle)jobTable.size();
    jobTable.emplace_back();
    jobTable[newHandle].handle = newHandle;
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


}
}
