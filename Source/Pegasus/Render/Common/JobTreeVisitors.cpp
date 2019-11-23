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

void ResourceStateBuilder::ApplyBarriers(const JobInstance* jobTable, const unsigned jobTableSize,
        const CanonicalJobPath& path, unsigned beginIndex, unsigned pathCount)
{
    GpuListLocation listLocation;
    listLocation.listId = path.GetId();
    for (unsigned i = 0; i < pathCount; ++i)
    {
        listLocation.listIndex = beginIndex + i;
        InternalJobHandle handle = path.GetCmdList()[i + beginIndex];
        PG_ASSERT((unsigned)handle < jobTableSize);
        const JobInstance& instance = jobTable[handle];
        for (auto& tableRef : instance.srvTables)
        {
            SetState(listLocation, ResourceGpuState::Srv, &(*tableRef));
        }
#if 1
        std::visit(overloaded {
            [this, &listLocation](const RootCmdData& d){
            },
            [this, &listLocation](const DrawCmdData& d){
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
#endif
    }
}

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

void CanonicalCmdListBuilder::FlushGpuStates()
{
    if (mBuildContext.listIndex == -1)
        return;   
}

bool CanonicalCmdListBuilder::OnPushed(InternalJobHandle handle, JobInstance& jobInstance)
{
    PG_ASSERTSTR(mStateTable[handle].state == State::Initial, "Impossible state: CanProcess shouldnt let this happen. Only things that have all dependencies done can be popped/pushed."); 
    PG_ASSERTSTR(mStateTable[handle].context.listIndex == -1, "Impossible list context: this resource has not been added yet to a list context");

    auto createNewList = [this]()
    {
        mBuildContext.listIndex = (int)mJobPaths.size();
        mBuildContext.listItemIndex = -1;
        mJobPaths.emplace_back();
        mJobPaths[mBuildContext.listIndex].SetId(mBuildContext.listIndex);
    };

    bool hasParentList = mStateTable[handle].parentListId != -1;
    if (!hasParentList)
    {
        FlushGpuStates();
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
