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
#include <unordered_map>
#include <stack>

namespace Pegasus
{
namespace Render
{

namespace Alloc
{
    class IAllocator;
}

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
    CopyDst
};

struct ResourceGpuStateDesc
{
    bool isRead : 1;
    bool isWrite : 1;
    static ResourceGpuStateDesc Get(ResourceGpuState state);
};

struct GpuListLocation
{
    unsigned listIndex = 0xffffffff;
    unsigned listItemIndex = 0xffffffff;
    bool operator==(const GpuListLocation& other) const
    {
        return listIndex == other.listIndex &&
            listItemIndex == other.listItemIndex;
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
            const GpuListLocation& initialLocation, unsigned endIndex);

    void UnapplyBarriers(const CanonicalJobPath& path, unsigned beginIndex, unsigned endIndex);

    const BarrierViolation* GetBarrierViolations(unsigned& outCount) const
    {
        outCount = (unsigned)mViolations.size();
        return mViolations.data();
    }

private:

    struct GpuListRange
    {
        unsigned listIndex;
        unsigned beginIndex;
        unsigned endIndex;

        bool operator==(const GpuListRange& other) const
        {
            return listIndex == other.listIndex && 
                beginIndex == other.beginIndex &&
                endIndex == other.endIndex;
        }
    };

    struct GpuListHasher
    {
        uint32_t operator()(const GpuListRange& range) const
        {
            uint64_t hh = 0ull;
            hh |= range.listIndex;
            hh |= ((uint64_t(range.beginIndex) & 0xffffull) << 32);
            hh |= ((uint64_t(range.endIndex) & 0xffffull) << 48);
            return std::hash<uint64_t>()(hh);
        };
    };

    struct SublistRecord
    {
        SublistRecord() {}

        SublistRecord(SublistRecord&& rr)
        {
            range = rr.range;
			refCount = rr.refCount;
            dependencies = std::move(rr.dependencies);
            barriers = std::move(rr.barriers);
        }

        GpuListRange range;
        unsigned refCount = 0u;
        std::vector<GpuListRange> dependencies;
        std::vector<ResourceBarrier> barriers;
    };

    struct GpuResourceStateRecord
    {
        LocationGpuState state;
        std::unordered_map<GpuListRange, std::vector<LocationGpuState> , GpuListHasher> usagesRanges;
    };

    void SetState(const GpuListRange& parentRange, GpuListLocation listLocation, ResourceGpuState newState, const IResource* resource);
    void SetState(const GpuListRange& parentRange, GpuListLocation listLocation, ResourceGpuState newState, const ResourceTable* resourceTable);
    void StoreResourceState(const GpuListRange& parentRange, const LocationGpuState& gpuState, const IResource* resource, bool checkViolation);
	void FlushResourceStates(const SublistRecord& record, bool applyInverse);

    std::vector<BarrierViolation> mViolations;
    std::vector<GpuResourceStateRecord> mStates;
    std::vector<ResourceBarrier> mBarriers;
    std::unordered_map<GpuListRange, SublistRecord, GpuListHasher> m_records;
    std::unordered_map<GpuListLocation, GpuListRange, GpuListLocationHasher> m_locationCache;
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
        mCmdList.push_back(handle);
    }

    struct Dependency
    {
        GpuListLocation location;
        unsigned sublistIndex = 0xffffffff;
        unsigned dstListItemIndex = 0xffffffff;
    };

    void AddDependency(const GpuListLocation& location, unsigned sublistIndex);
    void AddDependency(const Dependency& d);
    void QueryDependencies(int beginIndex, int endIndex, std::vector<Dependency>& outDependencies) const;

    std::vector<InternalJobHandle>& GetCmdList()
    {
        return mCmdList;
    }

    const std::vector<InternalJobHandle>& GetCmdList() const 
    {
        return mCmdList;
    }

private:

    unsigned mJobPathId = 0xffffffff;
    std::vector<Dependency> mDependencies;
    std::vector<InternalJobHandle> mCmdList;
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
        int sublistBaseIndex = -1;
    };

    struct NodeState
    {
        BuildContext context;
        int parentListId = -1;
        bool beginOfSublist = false;
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
