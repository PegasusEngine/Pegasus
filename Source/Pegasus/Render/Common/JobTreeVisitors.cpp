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

void CanonicalCmdListBuilder::Build(const GpuJob& rootJob)
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

void CanonicalCmdListBuilder::OnNoProcess(InternalJobHandle handle, JobInstance& jobInstance)
{
}

bool CanonicalCmdListBuilder::OnPushed(InternalJobHandle handle, JobInstance& jobInstance)
{
    PG_ASSERTSTR(mStateTable[handle].state != State::Popped, "Impossible state: CanProcess shouldnt let this happen. Only things that have all dependencies done can be popped."); 
    PG_ASSERTSTR(mStateTable[handle].context.listIndex == -1, "Impossible list context: this resource has not been added yet to a list context");

    //handle special cases first
    if (mStateTable[handle].state == State::Pushed)
    {
        PG_LOG('_ERR', "Found circular dependency.");
        return false;
    }

    auto createNewList = [this]()
    {
        mBuildContext.listIndex = (int)mJobPaths.size();
        mBuildContext.listItemIndex = -1;
        mJobPaths.emplace_back();
    };

    bool hasCurrentList = mBuildContext.listIndex == -1;
    if (!hasCurrentList || mJobPaths[mBuildContext.listIndex].Size() != mBuildContext.listItemIndex)
        createNewList();

    AddHandleToCurrList(handle);
    mBuildContextStack.push(mBuildContext);
    mStateTable[handle] = NodeState{ mBuildContext, State::Pushed };
    for (int depIndx = 1; depIndx < (int)jobInstance.dependenciesSorted.size(); ++depIndx)
    {
        auto depState = mStateTable[jobInstance.dependenciesSorted[depIndx]];
        PG_ASSERTSTR(depState.context.listItemIndex != -1, "Resource dependency state must've been processed already.");
        mJobPaths[handle].AddDependency(depState.context.listIndex, depState.context.listItemIndex);
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
