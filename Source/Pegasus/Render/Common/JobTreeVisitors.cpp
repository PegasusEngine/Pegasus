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
        case ResourceGpuState::Ds:
            d = { false, true };
            break;
        case ResourceGpuState::CopySrc:
            d = { true, false };
            break;
        case ResourceGpuState::CopyDst:
            d = { false, true };
            break;
        case ResourceGpuState::UavWrite:
            d = { true, true };
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
    if (mTable.GetState(mDomain, resource->GetStateId(), oldStateHandle))
        return mStates[oldStateHandle].state;

    return LocationGpuState();
}

void ResourceStateBuilder::StoreResourceState(
    unsigned parentListId,
    const LocationGpuState& gpuState, const IResource* resource, bool checkViolations)
{
    uintptr_t stateHandle = 0u;
    if (!mTable.GetState(mDomain, resource->GetStateId(), stateHandle))
    {
        stateHandle = (uintptr_t)mStates.size();
        mTable.StoreState(mDomain, resource->GetStateId(), stateHandle);
        mStates.emplace_back();
        mStates.back().usagesLists[parentListId] = std::move(std::vector<LocationGpuState>());
    }
    else
    {
        //validation
        if (checkViolations)
        {
            ResourceGpuStateDesc inStateDesc = ResourceGpuStateDesc::Get(gpuState.gpuState);
            for (auto r : mStates[stateHandle].usagesLists)
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
    mStates[stateHandle].usagesLists[parentListId].push_back(gpuState);
}

void ResourceStateBuilder::SetState(unsigned parentListId, GpuListLocation listLocation, ResourceGpuState newState, const IResource* resource)
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

    StoreResourceState(parentListId, newGpuState, resource, true);
}

void ResourceStateBuilder::SetState(unsigned parentListId, GpuListLocation listLocation, ResourceGpuState newState, const ResourceTable* resourceTable)
{
        for (auto resRef : resourceTable->GetConfig().resources)
            SetState(parentListId, listLocation, newState, &(*resRef));
}

void ResourceStateBuilder::SetState(unsigned parentListId, GpuListLocation listLocation, ResourceGpuState newState, const RenderTarget* rt)
{
    for (unsigned i = 0; i < rt->GetConfig().colorCount; ++i)
    {
        if (rt->GetConfig().colors[i] != nullptr)
            SetState(parentListId, listLocation, newState, &(*rt->GetConfig().colors[i]));
    }
    if (rt->GetConfig().depthStencil != nullptr)
        SetState(parentListId, listLocation, newState, &(*rt->GetConfig().depthStencil));
}

void ResourceStateBuilder::FlushResourceStates(const ListRecord& record, bool applyInverse)
{
    for (auto& barrier : record.barriers)
    {
        auto& gpuState = applyInverse ? barrier.from : barrier.to;
        StoreResourceState(record.listId, gpuState, barrier.resource, false);
    }
}

void ResourceStateBuilder::Reset()
{
    mStates.clear();
    mBarriers.clear();
    m_records.clear();
	mViolations.clear();

    mTable.RemoveDomain(mDomain);
    mDomain = mTable.CreateDomain();
}

void ResourceStateBuilder::ApplyBarriers(
        const JobInstance* jobTable, const unsigned jobTableSize,
        const CanonicalJobPath* jobPaths, const unsigned jobPathsSize, 
        unsigned listId)
{
    const CanonicalJobPath& path = jobPaths[listId];

    auto applyResourceStatesFromDeps = [&](const std::vector<unsigned>& dependenciesListIds)
    {
        for (int depIndex = (int)dependenciesListIds.size() - 1; depIndex >= 0; --depIndex )
        {
            auto recIt = m_records.find(dependenciesListIds[depIndex]);
            PG_ASSERT(recIt != m_records.end());
            FlushResourceStates(recIt->second, false);
        }
    };

    {
        auto listRecord = m_records.find(listId);
        if (listRecord != m_records.end())
        {
            for (auto dep : listRecord->second.dependenciesListIds)
            {
                auto depIt = m_records.find(dep);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;
            }
            ++listRecord->second.refCount;
            applyResourceStatesFromDeps(listRecord->second.dependenciesListIds);
            return;
        }
    }

    ListRecord listRecord;
    listRecord.refCount = 1u;
    listRecord.listId = listId; 
    {
        std::unordered_set<unsigned> traversed;
        std::vector<unsigned> pendingLists;
        pendingLists.push_back(listId);
        while (!pendingLists.empty())
        {
            unsigned currList = pendingLists.back();
            pendingLists.pop_back();

            std::vector<CanonicalJobPath::Dependency> outDeps;
            jobPaths[currList].QueryDependencies(0u, jobPaths[currList].Size(), outDeps);

            for (const auto& dep : outDeps)
            {
                listRecord.dependenciesListIds.push_back(dep.location.listIndex);
                auto depIt = m_records.find(dep.location.listIndex);
                PG_ASSERT(depIt != m_records.end());
                ++depIt->second.refCount;

                if (traversed.insert(dep.location.listIndex).second)
                    pendingLists.push_back(dep.location.listIndex);
            }
        }
    }

    applyResourceStatesFromDeps(listRecord.dependenciesListIds);

    GpuListLocation listLocation;
    listLocation.listItemIndex = 0u;
    listLocation.listIndex = listId;
    for (unsigned i = 0u; i < (unsigned)path.Size(); ++i)
    {
        listLocation.listItemIndex = i;
        InternalJobHandle handle = path.GetCmdList()[i].jobHandle;
        PG_ASSERT((unsigned)handle < jobTableSize);
        const JobInstance& instance = jobTable[handle];
        for (auto& tableRef : instance.srvTables)
        {
            SetState(listId, listLocation, ResourceGpuState::Srv, &(*tableRef));
        }
    
        for (auto& tableRef : instance.uavTables)
        {
            SetState(listId, listLocation, ResourceGpuState::Uav, &(*tableRef));
        }

        std::visit(JobVisitor::overloaded {
            [&](const RootCmdData& d){
            },
            [&](const DrawCmdData& d){
                if (d.rt == nullptr)
                    return;
                SetState(listId, listLocation, ResourceGpuState::Rt, &(*d.rt));
            },
            [&](const ComputeCmdData& d){
            },
            [&](const CopyCmdData& d){
                SetState(listId, listLocation, ResourceGpuState::CopySrc, &(*d.src));
                SetState(listId, listLocation, ResourceGpuState::CopyDst, &(*d.dst));
            },
            [&](const ClearRenderTargetCmdData& d){
                if (d.rt == nullptr)
                    return;
                SetState(listId, listLocation, ResourceGpuState::Rt, &(*d.rt));
            },
            [&](const DisplayCmdData& d){
                TextureRef t = d.display != nullptr ? const_cast<DisplayCmdData&>(d).display->GetTexture() : nullptr;
                if (t != nullptr)
                {
                    SetState(listId, listLocation, ResourceGpuState::Presentable, &(*t));
                }
            },
            [&](const GroupCmdData& d){
            }
        }, instance.data);

        for (auto& tableRef : instance.uavTables)
        {
            SetState(listId, listLocation, ResourceGpuState::UavWrite, &(*tableRef));
        }
    }

    listRecord.barriers = std::move(mBarriers);
    mBarriers = std::vector<ResourceBarrier>();
    m_records.insert(std::make_pair(listId, std::move(listRecord)));
}

void ResourceStateBuilder::UnapplyBarriers(const CanonicalJobPath& path)
{
    auto recIt = m_records.find(path.GetId());
    PG_ASSERT(recIt != m_records.end());
    PG_ASSERT(recIt->second.refCount != 0u);
    --recIt->second.refCount;
    FlushResourceStates(recIt->second, true);

    for (auto depListId : recIt->second.dependenciesListIds)
    {
        auto depIt = m_records.find(depListId);
        PG_ASSERT(depIt != m_records.end());
        PG_ASSERT(depIt->second.refCount != 0u);
        --depIt->second.refCount;
        FlushResourceStates(depIt->second, true);
    }
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

void CanonicalJobPath::AddDependency(const GpuListLocation& listLocation)
{
    PG_ASSERT(!mCmdList.empty());
    CanonicalJobPath::Dependency d;
    d.location = listLocation;
    d.ownerNodeIndex = (unsigned)(mCmdList.size()) - 1;
    AddDependency(d);
}

void CanonicalJobPath::AddDependency(const CanonicalJobPath::Dependency& dep)
{
    PG_ASSERT(dep.ownerNodeIndex < (unsigned)mCmdList.size());
    mCmdList[dep.ownerNodeIndex].dependencyIndices.push_back((unsigned)mDependencies.size());
    mDependencies.emplace_back(dep);
}

void CanonicalJobPath::QueryDependencies(int beginIndex, int endIndex, std::vector<CanonicalJobPath::Dependency>& outDependencies) const
{
    for (const auto& dep : mDependencies)
    {
        if ((int)dep.ownerNodeIndex >= beginIndex && (int)dep.ownerNodeIndex <= endIndex)
            outDependencies.push_back(dep);
    }
}

void CanonicalJobPath::AddBarrier(const ResourceBarrier& barrier)
{
    const bool hasSource = barrier.from.location.listIndex == GetId();
    const bool hasDest = barrier.to.location.listIndex == GetId();
    if (!hasSource && !hasDest)
        return;

    const bool isImmediate = hasSource && hasDest && (barrier.from.location.listItemIndex + 1) == barrier.to.location.listItemIndex;
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
    PG_ASSERTSTR(mStateTable[handle].context.listLocation.listIndex == InvalidIndex, "Impossible list context: this resource has not been added yet to a list context");

    auto createNewList = [this]()
    {
        mBuildContext.listLocation.listIndex = (int)mJobPaths.size();
        mBuildContext.listLocation.listItemIndex = 0u;
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
            PG_ASSERTSTR(depState.context.listLocation.listIndex != InvalidIndex, "Resource dependency state must've been processed already.");
            PG_ASSERTSTR(depState.context.listLocation.listItemIndex != InvalidIndex, "Resource dependency state must've been processed already.");
            mJobPaths[mBuildContext.listLocation.listIndex].AddDependency(depState.context.listLocation);
        }
    }

    auto applyBarriers = [&]()
    {
		mStateTable[handle].flushedBarriers = true;
        mGpuStateBuilder.ApplyBarriers(
            mJobTable, mJobCounts,
            mJobPaths.data(), (unsigned)mJobPaths.size(),
            mBuildContext.listLocation.listIndex);
    };

    if (jobInstance.childrenJobs.size() == 1u && CanProcess(jobInstance.childrenJobs[0], mJobTable[jobInstance.childrenJobs[0]]) && jobInstance.dependenciesSet.size() == 1u)
    {
        auto& nextChild = mStateTable[jobInstance.childrenJobs[0]];
		auto& nextChildInstance = mJobTable[jobInstance.childrenJobs[0]];
        nextChild.parentListId = mBuildContext.listLocation.listIndex;
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
        mGpuStateBuilder.UnapplyBarriers(mJobPaths[mBuildContext.listLocation.listIndex]);
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
