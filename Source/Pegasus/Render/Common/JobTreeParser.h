/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   JobTreeParser.h
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Job BFS and DFS parser

#pragma once

#include "InternalJobBuilder.h"
#include <queue>
#include <stack>

namespace Pegasus
{
namespace Render
{

template<typename VisitorT>
bool ParseRootJobBFS(const GpuJob& rootJob, VisitorT& visitor)
{
    auto& jobTable = rootJob.GetParent()->jobTable;

    if (jobTable.empty())
        return false;

    struct NodeState
    {
        InternalJobHandle handle;
    };

    visitor.OnBegin(jobTable.data(), (unsigned)jobTable.size());

    std::queue<NodeState> processQueue;
    processQueue.push(NodeState{ rootJob.GetInternalJobHandle() });
    if (!visitor.OnEnqueued(rootJob.GetInternalJobHandle(), jobTable[rootJob.GetInternalJobHandle()]))
        return false;

    while (!processQueue.empty())
    {
        auto& state = processQueue.front();
        auto& jobInstance = jobTable[state.handle];
        processQueue.pop();

        if (!visitor.CanProcess(state.handle, jobInstance))
        {
			if (!visitor.OnNoProcess(state.handle, jobInstance))
				return false;
            continue;
        }

        if (!visitor.OnDequeued(state.handle, jobInstance))
            return false;

        for (auto pHandle : jobInstance.childrenJobs)
        {
            if (!visitor.OnEnqueued(pHandle, jobTable[pHandle]))
                return false;

            processQueue.push(NodeState { pHandle });
        }
    }

    visitor.OnEnd();

    return true;
}

template<typename VisitorT>
bool ParseRootJobDFS(const GpuJob& rootJob, VisitorT& visitor)
{
    auto& jobTable = rootJob.GetParent()->jobTable;

    if (jobTable.empty())
        return false;

    struct NodeState
    {
        InternalJobHandle handle;
        bool childrenProcessed = false;
    };

    visitor.OnBegin(jobTable.data(), (unsigned)jobTable.size());

    std::stack<NodeState> processStack;
    processStack.push(NodeState{ rootJob.GetInternalJobHandle() });

    while (!processStack.empty())
    {
		auto& state = processStack.top();
        auto& jobInstance = jobTable[state.handle];


        if (!state.childrenProcessed)
        {
			if (!visitor.CanProcess(state.handle, jobInstance))
			{
				if (!visitor.OnNoProcess(state.handle, jobInstance))
					return false;
				processStack.pop();
				continue;
			}

            //begin stack visit of this parent
            if (!visitor.OnPushed(state.handle, jobInstance))
                return false;

            //Important: orden of operaitons matter.
            //After pushing into the process stack we cant access the state anymore.
            state.childrenProcessed = true;

            //push children into stack.
            for (auto pHandle : jobInstance.childrenJobs)
                processStack.push(NodeState { pHandle });
    
        }
        else
        {
            //end visit to this node.
            if (!visitor.OnPopped(state.handle, jobInstance))
                return false;

            processStack.pop();
        }
    }

    visitor.OnEnd();

    return true;
}

}
}
