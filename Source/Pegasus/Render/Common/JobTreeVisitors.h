/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   jobtreevisitors.h
//! \author kleber garcia
//! \date  september 8th
//! \brief  family of job tree visitors that generate il for cmd lists

#pragma once

#include "InternalJobBuilder.h"
#include "ResourceStateTable.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <stack>

namespace Pegasus
{

namespace Alloc
{
	class IAllocator;
}

namespace Render
{
class CanonicalJobPath;

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

enum class ResourceGpuState : unsigned
{
    Default,
    Uav,
    Srv,
    Cbv,
    Rt,
    Ds,
    CopySrc,
    CopyDst,
    Count
};

struct ResourceGpuStateDesc
{
    bool isRead : 1;
    bool isWrite : 1;
    static ResourceGpuStateDesc Get(ResourceGpuState state);
};

const unsigned InvalidIndex = 0xffffffff;

struct GpuListLocation
{
    unsigned listIndex = InvalidIndex;
    unsigned listItemIndex = InvalidIndex;
    bool operator==(const GpuListLocation& other) const
    {
        return listIndex == other.listIndex &&
            listItemIndex == other.listItemIndex;
    }

    bool isValid() const
    {
        return listIndex != InvalidIndex && listItemIndex != InvalidIndex;
    }
};

struct GpuListLocationHasher
{
    uint32_t operator()(const GpuListLocation& location) const
    {
        uint64_t hh = location.listIndex;
        hh |= (uint64_t(location.listItemIndex) << 32ull);
        return std::hash<uint64_t>()(hh);
    }
};

struct LocationGpuState
{
    GpuListLocation location;
    ResourceGpuState gpuState = ResourceGpuState::Default;
};

struct ResourceBarrier
{
    const IResource* resource = nullptr;
    LocationGpuState from;
    LocationGpuState to;

    bool isSameList() const
    {
        return from.location.listIndex == to.location.listIndex;
    }

    unsigned GetInterestedLists(unsigned indices[2]) const
    {
        if (isSameList())
        { 
            PG_ASSERT(to.location.isValid() && from.location.isValid());
            indices[0] = from.location.listIndex;
            return 1;
        }

        unsigned targetCount = 0u;
        if (from.location.isValid())
            indices[targetCount++] = from.location.listIndex;
        if (to.location.isValid())
            indices[targetCount++] = to.location.listIndex;
        return targetCount;
    }
};

struct BarrierViolation
{
    const IResource* resource = nullptr;
	LocationGpuState sourceLocation;
    std::vector<LocationGpuState> inFlightStates;

    BarrierViolation() = default;
    BarrierViolation(const BarrierViolation& other) = default;
    BarrierViolation(BarrierViolation&& other)
    {
        resource = other.resource;
        sourceLocation = other.sourceLocation;
        inFlightStates = std::move(other.inFlightStates);
    }
};

class ResourceStateBuilder
{
public:
    explicit ResourceStateBuilder(ResourceStateTable& table);

    ~ResourceStateBuilder();
    
	LocationGpuState GetResourceState(const IResource* resource) const;
	void Reset();
    void ApplyBarriers(const JobInstance* jobTable, const unsigned jobTableSize,
            const CanonicalJobPath* jobPaths, const unsigned jobPathsSize, 
            unsigned listId);

    void UnapplyBarriers(const CanonicalJobPath& path);

    const BarrierViolation* GetBarrierViolations(unsigned& outCount) const
    {
        outCount = (unsigned)mViolations.size();
        return mViolations.data();
    }

    void StoreBarriers(CanonicalJobPath* jobPaths, unsigned jobPathsSize) const;

private:

    struct ListRecord
    {
        ListRecord() {}

        ListRecord(ListRecord&& rr)
        {
            listId = rr.listId;
			refCount = rr.refCount;
            dependenciesListIds = std::move(rr.dependenciesListIds);
            barriers = std::move(rr.barriers);
        }

        unsigned listId = 0u;
        unsigned refCount = 0u;
        std::vector<unsigned> dependenciesListIds;
        std::vector<ResourceBarrier> barriers;
    };

    struct GpuResourceStateRecord
    {
        LocationGpuState state;
        std::unordered_map<unsigned, std::vector<LocationGpuState>> usagesLists;
    };

    void SetState(unsigned parentListId, GpuListLocation listLocation, ResourceGpuState newState, const IResource* resource);
    void SetState(unsigned parentListId, GpuListLocation listLocation, ResourceGpuState newState, const ResourceTable* resourceTable);
    void StoreResourceState(unsigned parentListId, const LocationGpuState& gpuState, const IResource* resource, bool checkViolation);
	void FlushResourceStates(const ListRecord& record, bool applyInverse);

    std::vector<BarrierViolation> mViolations;
    std::vector<GpuResourceStateRecord> mStates;
    std::vector<ResourceBarrier> mBarriers;
    std::unordered_map<unsigned, ListRecord> m_records;
    ResourceStateTable::Domain mDomain;
    ResourceStateTable& mTable;
};

class CanonicalJobPath
{
public:

    void SetId (unsigned id) { mJobPathId = id; }
    unsigned GetId() const { return mJobPathId; }
    void Clear() { mCmdList.clear(); }

    bool IsEmpty() const { return mCmdList.empty(); }

    int Size() const { return (int)mCmdList.size(); }

    void Add(InternalJobHandle handle)
    {
        mCmdList.emplace_back(Node{ handle, {} });
    }

    struct Dependency
    {
        GpuListLocation location;
        unsigned ownerNodeIndex = InvalidIndex;
    };

    enum class BarrierTiming
    {
        Begin, End, BeginAndEnd
    };

    struct GpuBarrier : public ResourceBarrier
    {
        BarrierTiming timing;
    };

    struct Node
    {
        InternalJobHandle jobHandle;
        std::vector<unsigned> dependencyIndices;
        std::vector<unsigned> preGpuBarrierIndices; //barriers before this node
        std::vector<unsigned> postGpuBarrierIndices; //barriers after this node
    };

    void AddDependency(const GpuListLocation& location);
    void AddDependency(const Dependency& d);
    void AddBarrier(const ResourceBarrier& barrier);

    void QueryDependencies(int beginIndex, int endIndex, std::vector<Dependency>& outDependencies) const;

    std::vector<Node>& GetCmdList()
    {
        return mCmdList;
    }

    const std::vector<Node>& GetCmdList() const 
    {
        return mCmdList;
    }

    const Dependency* GetDependencies(unsigned& sizeOut) const
    {
        sizeOut = (unsigned)mDependencies.size();
        return mDependencies.data();
    }

    const GpuBarrier* GetBarriers(unsigned& sizeOut) const
    {
        sizeOut = (unsigned)mBarriers.size();
        return mBarriers.data();
    }

private:

    unsigned mJobPathId = InvalidIndex;
    std::vector<Dependency> mDependencies;
    std::vector<GpuBarrier> mBarriers;
    std::vector<Node> mCmdList;
};

struct CanonicalCmdListResult
{
    const CanonicalJobPath* cmdLists = nullptr;
    unsigned cmdListsCounts = 0u;

    const InternalJobHandle* staleJobs = nullptr;
    unsigned staleJobCounts = 0u;
    
    const BarrierViolation* barrierViolations = nullptr;
    unsigned barrierViolationsCount = 0u;
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
        GpuListLocation listLocation;
    };

    struct NodeState
    {
        BuildContext context;
		bool flushedBarriers = false;
        int parentListId = -1;
        State state = State::Initial;
    };

    JobInstance* mJobTable;
    unsigned mJobCounts;
    BuildContext mBuildContext;
    std::stack<BuildContext> mBuildContextStack;
    std::vector<NodeState> mStateTable;
    std::vector<CanonicalJobPath> mJobPaths;
    std::unordered_set<InternalJobHandle> mWaitingJobs;
    std::vector<InternalJobHandle> mStaleJobs;
    ResourceStateBuilder mGpuStateBuilder;
    Pegasus::Alloc::IAllocator* mAllocator;
};

}
}
