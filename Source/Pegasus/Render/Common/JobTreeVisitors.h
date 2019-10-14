/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   jobtreevisitors.h
//! \author kleber garcia
//! \date  september 8th
//! \brief  family of job tree visitors that generate il for cmd lists

#include "InternalJobBuilder.h"
#include "ResourceStateTable.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <stack>

namespace Pegasus
{
namespace Render
{

namespace Alloc
{
    class IAllocator;
}

class ChildJobAccumulator
{
public:
    ChildJobAccumulator(std::vector<InternalJobHandle>& targetJobHandles)
        :  mCachedJobHandles(targetJobHandles) {}

    void OnBegin(JobInstance* jobTable, unsigned jobCounts)
    {
        mVisitedTable.resize(jobCounts, false);
    }

    void OnEnd() {}

    bool OnEnqueued(InternalJobHandle h, JobInstance& jobInstance)
    {
        mCachedJobHandles.push_back(h);
		return true;
    }

    bool OnDequeued(InternalJobHandle h, JobInstance& jobInstance)
    {
        mVisitedTable[h] = true;
        return true;
    }

    bool CanProcess(InternalJobHandle h, JobInstance& jobInstance) const
    {
        return !mVisitedTable[h];
    }

	bool OnNoProcess(InternalJobHandle h, JobInstance& jobInstance) { return true;  }
	
private:
    std::vector<InternalJobHandle>& mCachedJobHandles;
    std::vector<bool> mVisitedTable;
};

class CanonicalResourceTransition
{
    //todo: resource transitions
};

class CanonicalJobPath
{
public:

    void Clear() { mCmdList.clear(); }

    bool IsEmpty() const { return mCmdList.empty(); }

    int Size() const { return (int)mCmdList.size(); }

    void Add(InternalJobHandle handle)
    {
        mCmdList.push_back(handle);
    }

    void AddDependency(int srcListIndex, int srcListItemIndex);
    void AddDependency(int srcListIndex, int srcListItemIndex, int dstListItemIndex);

    std::vector<InternalJobHandle>& GetCmdList()
    {
        return mCmdList;
    }

private:
    struct Dependency
    {
        int srcListIndex = -1;
        int srcListItemIndex = -1;
        int dstListItemIndex = -1;
    };

    std::vector<Dependency> mDependencies;
    std::vector<InternalJobHandle> mCmdList;
};

struct CanonicalCmdListResult
{
    CanonicalJobPath* cmdLists = nullptr;
    unsigned cmdListsCounts = 0u;
    
};

class CanonicalCmdListBuilder
{
public:
    CanonicalCmdListBuilder(Pegasus::Alloc::IAllocator* allocator, ResourceStateTable& stateTable);

    std::vector<CanonicalJobPath>& GetCanonicalResults()
    {
        return mJobPaths;
    }

    //main
    void Build(const GpuJob& rootJob, CanonicalCmdListResult& result);
    void Reset();

    //overrides for visitor
    void OnBegin(JobInstance* jobTable, unsigned jobCounts);
    void OnEnd();
    bool OnPushed(InternalJobHandle handle, JobInstance& jobInstance);
    bool OnPopped(InternalJobHandle handle, JobInstance& jobInstance);
    bool CanProcess(InternalJobHandle handle, JobInstance& jobInstance); 
    bool OnNoProcess(InternalJobHandle handle, JobInstance& jobInstance); 

private:

    void AddHandleToCurrList(InternalJobHandle handle);

    enum class State
    {
        Initial,
        Pushed,
        Popped
    };

    struct BuildContext
    {
        int listIndex = -1; 
        int listItemIndex = -1;
    };

    struct NodeState
    {
        BuildContext context;
        int parentListId = -1;
        State state = State::Initial;
    };

    JobInstance* mJobTable;
    unsigned mJobCounts;
    BuildContext mBuildContext;
    std::stack<BuildContext> mBuildContextStack;
    std::vector<NodeState> mStateTable;
    std::vector<CanonicalJobPath> mJobPaths;
    std::vector<CanonicalResourceTransition> mResourceTransitions;
    std::unordered_set<InternalJobHandle> mWaitingJobs;
    Pegasus::Alloc::IAllocator* mAllocator;
    ResourceStateTable& mResourceStateTable;
    ResourceStateTable::Domain mBuildDomain;
};

}
}
