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
    
void ResourceTransitionSet::AddBarrier(unsigned resourceId, const ResourceBarrier& barrier)
{
    m_barriers[resourceId] = barrier;
}

bool ResourceTransitionSet::GetBarrier(unsigned resourceId, ResourceBarrier& barrier)
{
    auto it = m_barriers.find(resourceId);
    if (it == m_barriers.end())
        return false;

    barrier = it->second;
    return true;
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
	LocationGpuState gpuState;
    if (oldStateHandle != 0u)
        gpuState = mStates[oldStateHandle];
    return gpuState;
}

void ResourceStateBuilder::StoreResourceState(const LocationGpuState& gpuState, const IResource* resource)
{
    uintptr_t stateHandle = 0u;
    mTable.GetState(mDomain, resource->GetStateId(), stateHandle);
    if (stateHandle == 0u)
    {
        stateHandle = (uintptr_t)mStates.size();
        mTable.StoreState(mDomain, resource->GetStateId(), stateHandle);
        mStates.emplace_back();
    }

    mStates[stateHandle] = gpuState;
}

void ResourceStateBuilder::SetState(GpuListLocation listLocation, ResourceGpuState newState, const IResource* resource)
{
    LocationGpuState oldGpuState = GetResourceState(resource);
	LocationGpuState newGpuState;
    newGpuState.location = listLocation;
    newGpuState.gpuState = newState;

    if (oldGpuState.gpuState != newGpuState.gpuState)
    {
        StoreResourceState(newGpuState, resource);
        ResourceBarrier newBarrier;
        newBarrier.resourceStateId = resource->GetStateId();
        newBarrier.from = oldGpuState;
        newBarrier.to = newGpuState;
        mBarriers.emplace_back(newBarrier);
    }
}

void ResourceStateBuilder::SetState(GpuListLocation listLocation, ResourceGpuState newState, const ResourceTable* resourceTable)
{
        for (auto resRef : resourceTable->GetConfig().resources)
            SetState(listLocation, newState, &(*resRef));
}

void ResourceStateBuilder::ApplyBarriers(
        const JobInstance* jobTable, const unsigned jobTableSize,
        const CanonicalJobPath* jobPaths, const unsigned jobPathsSize, 
        const GpuListLocation& initialLocation, unsigned endIndex)
{
    const CanonicalJobPath& path = jobPaths[initialLocation.listIndex];

    GpuListRange range = { path.GetId(), initialLocation.listItemIndex, endIndex };
    {
        auto rangeRecord = m_records.find(range);
        if (rangeRecord != m_records.end())
        {
            ++rangeRecord->second.refCount;
            for (auto dep : rangeRecord->second.dependencies)
            {
                auto depIt = m_records.find(dep);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;
            }
            return;
        }
    }

    GpuListLocation listLocation = initialLocation;
    for (unsigned i = initialLocation.listItemIndex; i <= endIndex; ++i)
    {
        listLocation.listItemIndex = i;
        InternalJobHandle handle = path.GetCmdList()[i];
        PG_ASSERT((unsigned)handle < jobTableSize);
        const JobInstance& instance = jobTable[handle];
        for (auto& tableRef : instance.srvTables)
        {
            SetState(listLocation, ResourceGpuState::Srv, &(*tableRef));
        }
        std::visit(overloaded {
            [this, &listLocation](const RootCmdData& d){
            },
            [this, &listLocation](const DrawCmdData& d){
				if (d.rt == nullptr)
					return;

                for (auto& resource :  d.rt->GetConfig().colors)
                {
                    if (resource == nullptr)
                        continue;
                    SetState(listLocation, ResourceGpuState::Rt, &(*resource));
                }

                if (d.rt->GetConfig().depthStencil != nullptr)
                    SetState(listLocation, ResourceGpuState::Ds, &(*d.rt->GetConfig().depthStencil));
            },
            [this, &listLocation](const ComputeCmdData& d){
                for (auto& tableRef : d.uavTables)
                    SetState(listLocation, ResourceGpuState::Uav, &(*tableRef));
            },
            [this, &listLocation](const CopyCmdData& d){
            },
            [this, &listLocation](const DisplayCmdData& d){
            },
            [this, &listLocation](const GroupCmdData& d){
            }
        }, instance.data);
    }

    SublistRecord sublistRecord;
    sublistRecord.refCount = 1u;
    sublistRecord.range = range; 
    sublistRecord.barriers = std::move(mBarriers);

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
            std::vector<GpuListLocation> outDeps;
            jobPaths[r.listIndex].QueryDependencies(0u, r.endIndex, outDeps);

            for (const auto& loc : outDeps)
            {
                auto rangeIt = m_locationCache.find(loc);
                PG_ASSERT(rangeIt != m_locationCache.end());

                sublistRecord.dependencies.push_back(rangeIt->second);
                auto depIt = m_records.find(rangeIt->second);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;

                auto recordIt = m_records.find(rangeIt->second);
                PG_ASSERT(recordIt != m_records.end());
                ++recordIt->second.refCount;
                if (traversed.insert(rangeIt->second).second)
                    pendingRanges.push_back(rangeIt->second);
            }
        }
    }

    mBarriers = std::vector<ResourceBarrier>();
    m_records.insert(std::make_pair(range, std::move(sublistRecord)));
    m_locationCache.insert(std::make_pair(initialLocation, range));
}

void ResourceStateBuilder::UnapplyBarriers(const CanonicalJobPath& path, unsigned beginIndex, unsigned endIndex)
{
    GpuListRange range = { path.GetId(), beginIndex, endIndex };
    auto recIt = m_records.find(range);
    PG_ASSERT(recIt != m_records.end());
    for (auto depRange : recIt->second.dependencies)
    {
        auto depIt = m_records.find(depRange);
        PG_ASSERT(depIt != m_records.end());
        PG_ASSERT(depIt->second.refCount != 0u);
        --depIt->second.refCount;
    }

    PG_ASSERT(recIt->second.refCount != 0u);
    --recIt->second.refCount;

    GpuListLocation listLocation;
    listLocation.listIndex = path.GetId();
    listLocation.listItemIndex = beginIndex;
    PG_ASSERT(m_records.find(range) != m_records.end());
    PG_ASSERT(m_locationCache.find(listLocation) != m_locationCache.end());
    PG_ASSERT(m_locationCache[listLocation] == range);
}

void CanonicalJobPath::AddDependency(const GpuListLocation& listLocation)
{
    AddDependency(listLocation, ((int)(mCmdList.size()) - 1));
}

void CanonicalJobPath::AddDependency(const GpuListLocation& listLocation, int dstListItemIndex)
{
    Dependency dep = { listLocation,  dstListItemIndex };
    mDependencies.emplace_back(dep);
}


void CanonicalJobPath::QueryDependencies(int beginIndex, int endIndex, std::vector<GpuListLocation>& outDependencies) const
{
    for (const auto& dep : mDependencies)
    {
        if (dep.dstListItemIndex >= beginIndex && dep.dstListItemIndex <= endIndex)
            outDependencies.push_back(dep.srcListLocation);
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
    
}

void CanonicalCmdListBuilder::Reset()
{
    mJobPaths.clear();
    mStateTable.clear();
    mStaleJobs.clear();
    mWaitingJobs.clear();
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
            mJobPaths[mBuildContext.listLocation.listIndex].AddDependency(depState.context.listLocation);
        }
    }

    auto applyBarriers = [&]()
    {
        GpuListLocation initialLocation = mBuildContext.listLocation;;
        initialLocation.listItemIndex = (unsigned)mBuildContext.sublistBaseIndex;
        mGpuStateBuilder.ApplyBarriers(
            mJobTable, mJobCounts,
            mJobPaths.data(), (unsigned)mJobPaths.size(),
            initialLocation, (unsigned)mBuildContext.listLocation.listItemIndex);
    };

    //chose a child to pass down, lets just do for now the first child
    if (!jobInstance.childrenJobs.empty())
    {
        auto& nextChild = mStateTable[jobInstance.childrenJobs[0]];
        nextChild.parentListId = mBuildContext.listLocation.listIndex;
        if ((unsigned)jobInstance.childrenJobs.size() > 1u)
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
    if (jobInstance.childrenJobs.empty() || (unsigned)jobInstance.childrenJobs.size() > 1u)
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
