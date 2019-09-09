/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   jobtreevisitors.h
//! \author kleber garcia
//! \date  september 8th
//! \brief  family of job tree visitors that generate il for cmd lists

#include "JobTreeVisitors.h"
#include "JobTreeParser.h"

namespace Pegasus
{
namespace Render
{

void CanonicalJobPath::AddDependency(int srcListIndex, int srcListItemIndex)
{
    AddDependency(srcListIndex, srcListItemIndex, ((int)(mCmdList.size()) - 1));
}

void CanonicalJobPath::AddDependency(int srcListIndex, int srcListItemIndex, int dstListItemIndex)
{
    Dependency dep = { srcListIndex, srcListItemIndex,  dstListItemIndex };
    mDependencies.emplace_back(dep);
}

CanonicalCmdListBuilder::CanonicalCmdListBuilder(Pegasus::Alloc::IAllocator* allocator)
: mAllocator(allocator), mJobCounts(0u), mJobTable(nullptr)
{
}

void CanonicalCmdListBuilder::Build(const GpuJob& rootJob)
{
    auto& jobTable = rootJob.GetParent()->jobTable;
    if (jobTable.empty())
        return;

    mJobTable = jobTable.data();
    mStateTable.resize(jobTable.size());
    

    ProcessNode initNode = { BuildContext(), rootJob.GetInternalJobHandle() };

    mProcessNodes.push(initNode);

    while (!mProcessNodes.empty())
    {
        auto processNode = mProcessNodes.front();
        mProcessNodes.pop();

        mBuildContext = processNode.context;
        GpuJob newRootNode = { processNode.rootJobHandle, rootJob.GetParent() };
        ParseRootJobDFS(newRootNode, *this);
    }
}

void CanonicalCmdListBuilder::OnBegin(JobInstance* jobTable, unsigned jobCounts)
{
}

void CanonicalCmdListBuilder::OnEnd()
{
}

bool CanonicalCmdListBuilder::OnPushed(InternalJobHandle handle, JobInstance& jobInstance)
{
    //handle special cases first
    if (mStateTable[handle].state == State::Pushed)
    {
        PG_LOG('_ERR', "Found circular dependency.");
        return false;
    }
    else if (mStateTable[handle].state == State::Popped)
    {
        auto& nodeContext = mStateTable[handle].context;
        auto& nodeJobPath = mJobPaths[nodeContext.listIndex];
        if (nodeContext.listIndex != mBuildContext.listIndex)
        {
            nodeJobPath.AddDependency(mBuildContext.listIndex, mBuildContext.listItemIndex, nodeContext.listItemIndex);
        }

        mBuildContextStack.push(mBuildContext);
        return true;
    }

    auto createNewList = [this]()
    {
        mBuildContext.listIndex = (int)mJobPaths.size();
        mBuildContext.listItemIndex = -1;
        mJobPaths.emplace_back();
    };

    const bool hasJobList = mBuildContext.listIndex != -1;
    if (!hasJobList
    || mBuildContext.listItemIndex != mJobPaths[mBuildContext.listIndex].Size())
    {
        auto prevBuildContext = mBuildContext;
        createNewList();
        auto& currJobPath = mJobPaths[mBuildContext.listIndex];
        if (hasJobList)
        {
            currJobPath.AddDependency(prevBuildContext.listIndex, prevBuildContext.listItemIndex);
        }
    }

    AddHandleToCurrList(handle);
    mBuildContextStack.push(mBuildContext);
    mStateTable[handle] = NodeState{ mBuildContext, State::Pushed };
    return true;
}

bool CanonicalCmdListBuilder::OnPopped(InternalJobHandle handle, JobInstance& jobInstance)
{
    mStateTable[handle].state = State::Popped;
    mBuildContext = mBuildContextStack.top();
    mBuildContextStack.pop();
	return true;
}

bool CanonicalCmdListBuilder::CanProcess(InternalJobHandle handle, JobInstance& jobInstance)
{
    if (mStateTable[handle].state == State::Popped)
        return false;

    //check if all dependencies have been handled. If not, we push this into the queue
    for (InternalJobHandle parentDep : jobInstance.dependenciesSorted)
    {
        if (mStateTable[parentDep].state == State::Initial)
            return false;
    }
    
    return true;
}

void CanonicalCmdListBuilder::OnNoProcess(InternalJobHandle handle, JobInstance& jobInstance)
{
    if (mStateTable[handle].state != State::Initial)
        return; 

    //this only happens when the current node's dependencies are pending
    ProcessNode pn = { mBuildContext, handle };
    mProcessNodes.push(pn);
}

void CanonicalCmdListBuilder::AddHandleToCurrList(InternalJobHandle handle)
{
    PG_ASSERT(mBuildContext.listIndex >= 0 && mBuildContext.listIndex < (int)mJobPaths.size());
    CanonicalJobPath& jobPath = mJobPaths[mBuildContext.listIndex];

    PG_ASSERT((mBuildContext.listItemIndex + 1) == jobPath.Size());
    jobPath.Add(handle);
    
    ++mBuildContext.listItemIndex;
}


}
}
