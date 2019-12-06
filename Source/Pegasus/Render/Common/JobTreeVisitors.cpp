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
#include <variant>

// helper type for visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Pegasus
{
namespace Render
{

ResourceGpuStateDesc ResourceGpuStateDesc::Get(ResourceGpuState state)
{
    ResourceGpuStateDesc d = {};
    switch(state)
    {
            //   isRead , isWrite
        case ResourceGpuState::Default:
            d = { false, false };
            break;
        case ResourceGpuState::Uav:
            d = { true, true };
            break;
        case ResourceGpuState::Srv:
            d = { true, false };
            break;
        case ResourceGpuState::Cbv:
            d = { true, false };
            break;
        case ResourceGpuState::Rt:
            d = { false, true };
            break;
        case ResourceGpuState::Ds:
            d = { false, true };
            break;
        case ResourceGpuState::CopySrc:
            d = { true, false };
            break;
        case ResourceGpuState::CopyDst:
            d = { false, true };
            break;
        default:
            PG_FAILSTR("Invalid ResourceGpuState %d", state);
    }

    return d;
}

static bool areStatesCompatible(ResourceGpuState currentState, ResourceGpuState inFlightState)
{
    auto currDesc = ResourceGpuStateDesc::Get(currentState);
    auto inFlightDesc = ResourceGpuStateDesc::Get(currentState);

    return ((currDesc.isRead && !currDesc.isWrite) && (inFlightDesc.isRead && !inFlightDesc.isWrite)) ||
           (!currDesc.isRead && !currDesc.isWrite) ||
           (!inFlightDesc.isRead && !inFlightDesc.isWrite);
}
    
ResourceStateBuilder::ResourceStateBuilder(ResourceStateTable& table)
    : mDomain(table.CreateDomain()), mTable(table)
{
}

ResourceStateBuilder::~ResourceStateBuilder()
{
    mTable.RemoveDomain(mDomain);
}
    
LocationGpuState ResourceStateBuilder::GetResourceState(const IResource* resource) const
{
    uintptr_t oldStateHandle = 0u;
    mTable.GetState(mDomain, resource->GetStateId(), oldStateHandle);
    if (oldStateHandle != 0u)
        return mStates[oldStateHandle - 1u].state;

    return LocationGpuState();
}

void ResourceStateBuilder::StoreResourceState(
    const GpuListRange& parentRange,
    const LocationGpuState& gpuState, const IResource* resource, bool checkViolations)
{
    uintptr_t stateHandle = 0u;
    mTable.GetState(mDomain, resource->GetStateId(), stateHandle);
    if (stateHandle == 0u)
    {
        stateHandle = (uintptr_t)mStates.size();
        mTable.StoreState(mDomain, resource->GetStateId(), stateHandle + 1u);
        mStates.emplace_back();
        mStates.back().usagesRanges[parentRange] = std::move(std::vector<LocationGpuState>());
    }
    else
    {
        stateHandle = stateHandle - 1u;
        //validation
        if (checkViolations)
        {
            ResourceGpuStateDesc inStateDesc = ResourceGpuStateDesc::Get(gpuState.gpuState);
            for (auto r : mStates[stateHandle].usagesRanges)
            {
                auto recIt = m_records.find(r.first);
                if (recIt == m_records.end()) //means its the current list
                    continue;

                if (recIt->second.refCount == 0u)//means this resource is being used by another list
                {
                    BarrierViolation possibleViolation;
                    for (auto s : r.second)
                    {
                        if (areStatesCompatible(gpuState.gpuState, s.gpuState))
                            continue;

                        possibleViolation.inFlightStates.push_back(s);
                    }

                    if (!possibleViolation.inFlightStates.empty())
                    {
                        possibleViolation.resource = resource;
                        possibleViolation.sourceLocation = gpuState;
                        mViolations.emplace_back(std::move(possibleViolation));
                    }
                }
            }
        }
    }

    mStates[stateHandle].state = gpuState;
    mStates[stateHandle].usagesRanges[parentRange].push_back(gpuState);
}

void ResourceStateBuilder::SetState(const GpuListRange& parentRange, GpuListLocation listLocation, ResourceGpuState newState, const IResource* resource)
{
    LocationGpuState oldGpuState = GetResourceState(resource);
    LocationGpuState newGpuState;
    newGpuState.location = listLocation;
    newGpuState.gpuState = newState;
    
    ResourceBarrier newBarrier;
    newBarrier.resource = resource;
    newBarrier.from = oldGpuState;
    newBarrier.to = newGpuState;
    if (oldGpuState.gpuState != newGpuState.gpuState)
    {
        mBarriers.emplace_back(newBarrier);
    }

    StoreResourceState(parentRange, newGpuState, resource, true);
}

void ResourceStateBuilder::SetState(const GpuListRange& parentRange, GpuListLocation listLocation, ResourceGpuState newState, const ResourceTable* resourceTable)
{
        for (auto resRef : resourceTable->GetConfig().resources)
            SetState(parentRange, listLocation, newState, &(*resRef));
}

void ResourceStateBuilder::FlushResourceStates(const SublistRecord& record, bool applyInverse)
{
    for (auto& barrier : record.barriers)
    {
        auto& gpuState = applyInverse ? barrier.from : barrier.to;
        StoreResourceState(record.range, gpuState, barrier.resource, false);
    }
}

void ResourceStateBuilder::Reset()
{
    mStates.clear();
    mBarriers.clear();
    m_records.clear();
    m_locationCache.clear();
	mViolations.clear();

    mTable.RemoveDomain(mDomain);
    mDomain = mTable.CreateDomain();
}

void ResourceStateBuilder::ApplyBarriers(
        const JobInstance* jobTable, const unsigned jobTableSize,
        const CanonicalJobPath* jobPaths, const unsigned jobPathsSize, 
        const GpuListLocation& initialLocation, unsigned endIndex)
{
    const CanonicalJobPath& path = jobPaths[initialLocation.listIndex];

    auto applyResourceStatesFromDeps = [&](const std::vector<GpuListRange>& dependencies)
    {
        for (int depIndex = (int)dependencies.size() - 1; depIndex >= 0; --depIndex )
        {
            auto recIt = m_records.find(dependencies[depIndex]);
            PG_ASSERT(recIt != m_records.end());
            FlushResourceStates(recIt->second, false);
        }
    };

    GpuListRange range = { path.GetId(), initialLocation.listItemIndex, endIndex };
    {
        auto rangeRecord = m_records.find(range);
        if (rangeRecord != m_records.end())
        {
            for (auto dep : rangeRecord->second.dependencies)
            {
                auto depIt = m_records.find(dep);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;
            }
            ++rangeRecord->second.refCount;
            applyResourceStatesFromDeps(rangeRecord->second.dependencies);
            return;
        }
    }

    SublistRecord sublistRecord;
    sublistRecord.refCount = 1u;
    sublistRecord.range = range; 
    {
        GpuListRange r;
        r.listIndex = initialLocation.listIndex;
        r.beginIndex = initialLocation.listItemIndex;
        r.endIndex = endIndex;
        std::unordered_set<GpuListRange, GpuListHasher> traversed;
        std::vector<GpuListRange> pendingRanges;
        pendingRanges.push_back(r);
        while (!pendingRanges.empty())
        {
            r = pendingRanges.back();
            pendingRanges.pop_back();

            PG_ASSERT(r.listIndex < jobPathsSize);
            std::vector<CanonicalJobPath::Dependency> outDeps;
            jobPaths[r.listIndex].QueryDependencies(0u, r.endIndex, outDeps);

            for (const auto& dep : outDeps)
            {
                GpuListLocation loc = dep.location;
                loc.listItemIndex = dep.sublistIndex;
                
                auto rangeIt = m_locationCache.find(loc);
                PG_ASSERT(rangeIt != m_locationCache.end());

                sublistRecord.dependencies.push_back(rangeIt->second);
                auto depIt = m_records.find(rangeIt->second);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;

                if (traversed.insert(rangeIt->second).second)
                    pendingRanges.push_back(rangeIt->second);
            }
        }
    }

    applyResourceStatesFromDeps(sublistRecord.dependencies);

    GpuListLocation listLocation = initialLocation;
    for (unsigned i = initialLocation.listItemIndex; i <= endIndex; ++i)
    {
        listLocation.listItemIndex = i;
        InternalJobHandle handle = path.GetCmdList()[i].jobHandle;
        PG_ASSERT((unsigned)handle < jobTableSize);
        const JobInstance& instance = jobTable[handle];
        for (auto& tableRef : instance.srvTables)
        {
            SetState(range, listLocation, ResourceGpuState::Srv, &(*tableRef));
        }
        std::visit(overloaded {
            [&](const RootCmdData& d){
            },
            [&](const DrawCmdData& d){
                if (d.rt == nullptr)
                    return;

                for (auto& resource :  d.rt->GetConfig().colors)
                {
                    if (resource == nullptr)
                        continue;
                    SetState(range, listLocation, ResourceGpuState::Rt, &(*resource));
                }

                if (d.rt->GetConfig().depthStencil != nullptr)
                    SetState(range, listLocation, ResourceGpuState::Ds, &(*d.rt->GetConfig().depthStencil));
            },
            [&](const ComputeCmdData& d){
                for (auto& tableRef : d.uavTables)
                    SetState(range, listLocation, ResourceGpuState::Uav, &(*tableRef));
            },
            [&](const CopyCmdData& d){
                SetState(range, listLocation, ResourceGpuState::CopySrc, &(*d.src));
                SetState(range, listLocation, ResourceGpuState::CopyDst, &(*d.dst));
            },
            [&](const DisplayCmdData& d){
            },
            [&](const GroupCmdData& d){
            }
        }, instance.data);
    }

    sublistRecord.barriers = std::move(mBarriers);
    mBarriers = std::vector<ResourceBarrier>();
    m_records.insert(std::make_pair(range, std::move(sublistRecord)));
    PG_ASSERT(m_locationCache.find(initialLocation) == m_locationCache.end());
    m_locationCache.insert(std::make_pair(initialLocation, range));
}

void ResourceStateBuilder::UnapplyBarriers(const CanonicalJobPath& path, unsigned beginIndex, unsigned endIndex)
{
    GpuListRange range = { path.GetId(), beginIndex, endIndex };
    auto recIt = m_records.find(range);
    PG_ASSERT(recIt != m_records.end());
    PG_ASSERT(recIt->second.refCount != 0u);
    --recIt->second.refCount;
    FlushResourceStates(recIt->second, true);

    for (auto depRange : recIt->second.dependencies)
    {
        auto depIt = m_records.find(depRange);
        PG_ASSERT(depIt != m_records.end());
        PG_ASSERT(depIt->second.refCount != 0u);
        --depIt->second.refCount;
        FlushResourceStates(depIt->second, true);
    }

    GpuListLocation listLocation;
    listLocation.listIndex = path.GetId();
    listLocation.listItemIndex = beginIndex;
    PG_ASSERT(m_records.find(range) != m_records.end());
    PG_ASSERT(m_locationCache.find(listLocation) != m_locationCache.end());
    PG_ASSERT(m_locationCache[listLocation] == range);
}

void ResourceStateBuilder::StoreBarriers(CanonicalJobPath* jobPaths, unsigned jobPathsSize) const
{
    //only process barriers if there are no violations
    if (!mViolations.empty())
        return;

    for (const auto& recIt : m_records)
    {
        for (const auto& barrier : recIt.second.barriers)
        {
            unsigned targetJobs[2];
            unsigned targetJobCounts = barrier.GetInterestedLists(targetJobs);
            for (unsigned i = 0; i < targetJobCounts; ++i)
            {
                unsigned jobIndex = targetJobs[i];
                PG_ASSERT(jobIndex < jobPathsSize);
                jobPaths[jobIndex].AddBarrier(barrier);
            }
        }
    }
}

void CanonicalJobPath::AddDependency(const GpuListLocation& listLocation, unsigned sublistIndex)
{
    PG_ASSERT(!mCmdList.empty());
    CanonicalJobPath::Dependency d;
    d.location = listLocation;
    d.sublistIndex = sublistIndex;
    d.dstListItemIndex = (unsigned)(mCmdList.size()) - 1;
    AddDependency(d);
}

void CanonicalJobPath::AddDependency(const CanonicalJobPath::Dependency& dep)
{
    PG_ASSERT(dep.dstListItemIndex < (unsigned)mCmdList.size());
    mCmdList[dep.dstListItemIndex].dependencyIndices.push_back((unsigned)mDependencies.size());
    mDependencies.emplace_back(dep);
}

void CanonicalJobPath::QueryDependencies(int beginIndex, int endIndex, std::vector<CanonicalJobPath::Dependency>& outDependencies) const
{
    for (const auto& dep : mDependencies)
    {
        if ((int)dep.dstListItemIndex >= beginIndex && (int)dep.dstListItemIndex <= endIndex)
            outDependencies.push_back(dep);
    }
}

void CanonicalJobPath::AddBarrier(const ResourceBarrier& barrier)
{
    const bool hasSource = barrier.from.location.listIndex == GetId();
    const bool hasDest = barrier.to.location.listIndex == GetId();
    if (!hasSource && !hasDest)
        return;

    const bool isImmediate = hasSource && hasDest && barrier.from.location.listItemIndex == (barrier.to.location.listItemIndex + 1);
	GpuBarrier newBarrier = {};
	newBarrier.resource = barrier.resource;
	newBarrier.to = barrier.to;	
	newBarrier.from = barrier.from;

    if (isImmediate)
    {
        newBarrier.timing = CanonicalJobPath::BarrierTiming::BeginAndEnd;
        PG_ASSERT(newBarrier.to.location.listItemIndex < (unsigned)mCmdList.size());
        mCmdList[newBarrier.to.location.listItemIndex].preGpuBarrierIndices.push_back((unsigned)mBarriers.size());
        mBarriers.push_back(newBarrier);
        return;
    }
    
    if (hasSource)
    {
        newBarrier.timing = CanonicalJobPath::BarrierTiming::Begin;
        mCmdList[newBarrier.from.location.listItemIndex].postGpuBarrierIndices.push_back((unsigned)mBarriers.size());
		mBarriers.push_back(newBarrier);
    }

    if (hasDest)
    {
        newBarrier.timing = CanonicalJobPath::BarrierTiming::End;
        mCmdList[newBarrier.to.location.listItemIndex].preGpuBarrierIndices.push_back((unsigned)mBarriers.size());
		mBarriers.push_back(newBarrier);
    }

}

CanonicalCmdListBuilder::CanonicalCmdListBuilder(Pegasus::Alloc::IAllocator* allocator, ResourceStateTable& stateTable)
: mAllocator(allocator), mJobCounts(0u), mJobTable(nullptr), mGpuStateBuilder(stateTable)
{
}

void CanonicalCmdListBuilder::Build(const GpuJob& rootJob, CanonicalCmdListResult& result)
{
    auto& jobTable = rootJob.GetParent()->jobTable;
    if (jobTable.empty())
        return;

    Reset();
    mJobTable = jobTable.data();
    mJobCounts = (unsigned)jobTable.size();
    mStateTable.resize(jobTable.size());
    ParseRootJobDFS(rootJob, *this);

    mStaleJobs.reserve(mWaitingJobs.size());
    for (InternalJobHandle waitingJob : mWaitingJobs)
    {
        if (mStateTable[waitingJob].state != State::Popped)
        {
            mStaleJobs.push_back(waitingJob);
        }
    }

    mGpuStateBuilder.StoreBarriers(mJobPaths.data(), (unsigned)mJobPaths.size());

    result = {};

    if (!mJobPaths.empty())
    {
        result.cmdLists = mJobPaths.data();
        result.cmdListsCounts = (unsigned)mJobPaths.size();
    }

    if (!mStaleJobs.empty())
    {
        result.staleJobs = mStaleJobs.data();
        result.staleJobCounts = mStaleJobs.size();
    }

    result.barrierViolations = mGpuStateBuilder.GetBarrierViolations(result.barrierViolationsCount);
}

void CanonicalCmdListBuilder::Reset()
{
    mJobPaths.clear();
    mStateTable.clear();
    mStaleJobs.clear();
    mWaitingJobs.clear();
    mGpuStateBuilder.Reset();
    mBuildContextStack = std::stack<BuildContext>();
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
    PG_ASSERTSTR(mStateTable[handle].state == State::Initial, "Impossible state: CanProcess shouldnt let this happen. Only things that have all dependencies done can be popped/pushed."); 
    PG_ASSERTSTR(mStateTable[handle].context.listLocation.listIndex == 0xffffffff, "Impossible list context: this resource has not been added yet to a list context");

    auto createNewList = [this]()
    {
        mBuildContext.listLocation.listIndex = (int)mJobPaths.size();
        mBuildContext.listLocation.listItemIndex = 0u;
        mBuildContext.sublistBaseIndex = 0u;
        mJobPaths.emplace_back();
        mJobPaths[mBuildContext.listLocation.listIndex].SetId(mBuildContext.listLocation.listIndex);
    };

    bool hasParentList = mStateTable[handle].parentListId != -1;
    if (!hasParentList)
    {
        createNewList();
    }
    else
    {
        mBuildContext.listLocation.listIndex = (unsigned)mStateTable[handle].parentListId;
        mBuildContext.listLocation.listItemIndex = (unsigned)mJobPaths[mBuildContext.listLocation.listIndex].Size();
        if (mStateTable[handle].beginOfSublist)
        {
            mBuildContext.sublistBaseIndex = mBuildContext.listLocation.listItemIndex;
        }
    }

    AddHandleToCurrList(handle);
    
    mBuildContextStack.push(mBuildContext);
    mStateTable[handle].context = mBuildContext;
    mStateTable[handle].state = State::Pushed;
    for (int depIndx = 0; depIndx < (int)jobInstance.dependenciesSorted.size(); ++depIndx)
    {
        auto depState = mStateTable[jobInstance.dependenciesSorted[depIndx]];
        if (depState.context.listLocation.listIndex != mBuildContext.listLocation.listIndex)
        {
            PG_ASSERTSTR(depState.context.listLocation.listIndex != 0xffffffff, "Resource dependency state must've been processed already.");
            PG_ASSERTSTR(depState.context.listLocation.listItemIndex != 0xffffffff, "Resource dependency state must've been processed already.");
            PG_ASSERTSTR(depState.context.sublistBaseIndex != 0xffffffff, "Resource dependency state must've been processed already.");
            mJobPaths[mBuildContext.listLocation.listIndex].AddDependency(depState.context.listLocation, depState.context.sublistBaseIndex);
        }
    }

    auto applyBarriers = [&]()
    {
		mStateTable[handle].flushedBarriers = true;
        GpuListLocation initialLocation = mBuildContext.listLocation;;
        initialLocation.listItemIndex = (unsigned)mBuildContext.sublistBaseIndex;
        mGpuStateBuilder.ApplyBarriers(
            mJobTable, mJobCounts,
            mJobPaths.data(), (unsigned)mJobPaths.size(),
            initialLocation, (unsigned)mBuildContext.listLocation.listItemIndex);
    };

    unsigned availableChildJobs = 0;
    for (auto j : jobInstance.childrenJobs)
    {
        availableChildJobs += CanProcess(j, mJobTable[j]) ? 1 : 0;
    }

    //chose a child to pass down, lets just do for now the first child
    if (availableChildJobs != 0u)
    {
        auto& nextChild = mStateTable[jobInstance.childrenJobs[0]];
		auto& nextChildInstance = mJobTable[jobInstance.childrenJobs[0]];
        nextChild.parentListId = mBuildContext.listLocation.listIndex;
        if ((unsigned)jobInstance.childrenJobs.size() > 1u || nextChildInstance.dependenciesSet.size() > 1u)
        {
            applyBarriers();
            nextChild.beginOfSublist = true;
        }
    }
    else
    {
            applyBarriers();
    }

    return true;
}

bool CanonicalCmdListBuilder::OnPopped(InternalJobHandle handle, JobInstance& jobInstance)
{
    mBuildContext = mBuildContextStack.top();
    if (mStateTable[handle].flushedBarriers)
    {
        mGpuStateBuilder.UnapplyBarriers(mJobPaths[mBuildContext.listLocation.listIndex],
            (unsigned)mBuildContext.sublistBaseIndex,
            (unsigned)mBuildContext.listLocation.listItemIndex);
    }

    mStateTable[handle].state = State::Popped;
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
    PG_ASSERT(mBuildContext.listLocation.listIndex >= 0 && mBuildContext.listLocation.listIndex < (int)mJobPaths.size());
    CanonicalJobPath& jobPath = mJobPaths[mBuildContext.listLocation.listIndex];

    PG_ASSERT((mBuildContext.listLocation.listItemIndex) == jobPath.Size());
    jobPath.Add(handle);
}


}
}
