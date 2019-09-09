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
#include <vector>
#include <queue>
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

    void OnNoProcess(InternalJobHandle h, JobInstance& jobInstance) {}
	
private:
    std::vector<InternalJobHandle>& mCachedJobHandles;
    std::vector<bool> mVisitedTable;
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
        int dstLitsItemIndex = -1;
    };

    std::vector<Dependency> mDependencies;
    std::vector<InternalJobHandle> mCmdList;
};

class CanonicalCmdListBuilder
{
public:
    CanonicalCmdListBuilder(Pegasus::Alloc::IAllocator* allocator);

    std::vector<CanonicalJobPath>& GetCanonicalResults()
    {
        return mJobPaths;
    }

    //main
    void Build(const GpuJob& rootJob);

    //overrides for visitor
    void OnBegin(JobInstance* jobTable, unsigned jobCounts);
    void OnEnd();
    bool OnPushed(InternalJobHandle handle, JobInstance& jobInstance);
    bool OnPopped(InternalJobHandle handle, JobInstance& jobInstance);
    bool CanProcess(InternalJobHandle handle, JobInstance& jobInstance); 
    void OnNoProcess(InternalJobHandle handle, JobInstance& jobInstance); 

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

    struct ProcessNode
    {
        BuildContext context;
        InternalJobHandle rootJobHandle;
    };

    struct NodeState
    {
        BuildContext context;
        State state = State::Initial;
    };

    JobInstance* mJobTable;
    unsigned mJobCounts;
    BuildContext mBuildContext;
    std::stack<BuildContext> mBuildContextStack;
    std::vector<NodeState> mStateTable;
    std::queue<ProcessNode> mProcessNodes;
    std::vector<CanonicalJobPath> mJobPaths;
    Pegasus::Alloc::IAllocator* mAllocator;
};

}
}
