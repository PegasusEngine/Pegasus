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

CanonicalCmdListBuilder::CanonicalCmdListBuilder(Pegasus::Alloc::IAllocator* allocator, ResourceStateTable& stateTable)
: mAllocator(allocator), mJobCounts(0u), mJobTable(nullptr), mResourceStateTable(stateTable)
{
}

void CanonicalCmdListBuilder::Build(const GpuJob& rootJob, CanonicalCmdListResult& result)
{
    auto& jobTable = rootJob.GetParent()->jobTable;
    if (jobTable.empty())
        return;

    Reset();

    mJobTable = jobTable.data();
    mStateTable.resize(jobTable.size());
    mBuildDomain = mResourceStateTable.CreateDomain();
    ParseRootJobDFS(rootJob, *this);
    mResourceStateTable.RemoveDomain(mBuildDomain);
    mBuildDomain = ResourceStateTable::Domain();

    for (InternalJobHandle waitingJobs : mWaitingJobs)
    {
        if (mStateTable[waitingJobs].state != State::Popped)
        {
            PG_LOG('_ERR', "Circular dependency found. Stale job found");
        }
    }

    result = {};
    if (!mJobPaths.empty())
    {
        result.cmdLists = mJobPaths.data();
        result.cmdListsCounts = (unsigned)mJobPaths.size();
    }
    
}

void CanonicalCmdListBuilder::Reset()
{
    mJobPaths.clear();
    mStateTable.clear();
    mBuildContextStack = std::stack<BuildContext>();
    mResourceTransitions.clear();
    mJobTable = nullptr;
    mJobCounts = 0u;
}

void CanonicalCmdListBuilder::OnBegin(JobInstance* jobTable, unsigned jobCounts)
{
}

void CanonicalCmdListBuilder::OnEnd()
{
}

bool CanonicalCmdListBuilder::OnNoProcess(InternalJobHandle handle, JobInstance& jobInstance)
{
    mWaitingJobs.insert(handle);
    return true;
}

bool CanonicalCmdListBuilder::OnPushed(InternalJobHandle handle, JobInstance& jobInstance)
{
    PG_ASSERTSTR(mStateTable[handle].state != State::Popped, "Impossible state: CanProcess shouldnt let this happen. Only things that have all dependencies done can be popped."); 
    PG_ASSERTSTR(mStateTable[handle].context.listIndex == -1, "Impossible list context: this resource has not been added yet to a list context");

    //handle special cases first
    if (mStateTable[handle].state == State::Pushed)
    {
        PG_LOG('_ERR', "Found direct circular dependency.");
        return false;
    }

    auto createNewList = [this]()
    {
        mBuildContext.listIndex = (int)mJobPaths.size();
        mBuildContext.listItemIndex = -1;
        mJobPaths.emplace_back();
    };

    bool hasParentList = mStateTable[handle].parentListId != -1;
    if (!hasParentList)
    {
        createNewList();
    }
    else
    {
        mBuildContext.listIndex = mStateTable[handle].parentListId;
		mBuildContext.listItemIndex = mJobPaths[mBuildContext.listIndex].Size() - 1;
    }

    AddHandleToCurrList(handle);
    mBuildContextStack.push(mBuildContext);
    mStateTable[handle].context = mBuildContext;
    mStateTable[handle].state = State::Pushed;
    for (int depIndx = 0; depIndx < (int)jobInstance.dependenciesSorted.size(); ++depIndx)
    {
        auto depState = mStateTable[jobInstance.dependenciesSorted[depIndx]];
        if (depState.context.listIndex != mBuildContext.listIndex)
        {
            PG_ASSERTSTR(depState.context.listIndex != -1, "Resource dependency state must've been processed already.");
            PG_ASSERTSTR(depState.context.listItemIndex != -1, "Resource dependency state must've been processed already.");
            mJobPaths[mBuildContext.listIndex].AddDependency(depState.context.listIndex, depState.context.listItemIndex);
        }
    }

    //chose a child to pass down, lets just do for now the first child
    if (!jobInstance.childrenJobs.empty())
    {
        mStateTable[jobInstance.childrenJobs[0]].parentListId = mBuildContext.listIndex;
    }

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
