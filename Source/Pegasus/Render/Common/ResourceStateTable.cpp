/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ResourceStateTable.cpp
//! \author Kleber Garcia
//! \date   September 3rd 2019
//! \brief  Resource state tracker for resources in a queue

#include "ResourceStateTable.h"

namespace Pegasus
{
namespace Render
{

ResourceStateTable::ResourceStateTable()
: mStateCounts(0u)
{
    //just reserve a few domains for starters
    mDomains.reserve(16u);
}

ResourceStateTable::Domain ResourceStateTable::CreateDomain()
{
    Domain newDomain;

    if (mEmptyDomains.empty())
    {
        newDomain.id = (int)mDomains.size();
        mDomains.emplace_back();
    }
    else
    {
        newDomain.id = mEmptyDomains.back();
        mEmptyDomains.pop_back();
        PG_ASSERT(newDomain.id < (int)mDomains.size());
    }

    auto& newInfo = mDomains.back();
    PG_ASSERT(!newInfo.valid && newInfo.states.empty());
    newInfo.valid = true;
    newInfo.states.resize(mStateCounts, 0u);
    return newDomain;
}

void ResourceStateTable::RemoveDomain(ResourceStateTable::Domain d)
{
    PG_ASSERT(d.id >= 0 && d.id < (int)mDomains.size());
    if(d.id < 0 && d.id >= (int)mDomains.size())
        return;

    auto& info = mDomains[d.id];
    info = DomainInfo();
    mEmptyDomains.push_back(d.id);
}

int ResourceStateTable::CreateStateSlot()
{
    int newStateSlot;

    if (mEmptyStateSlotIdx.empty())
    {
        newStateSlot = (int)mStateCounts;
        for (DomainInfo& domain : mDomains)
        {
            PG_ASSERT((unsigned)domain.states.size() == mStateCounts);
            domain.states.emplace_back();
        }

        ++mStateCounts;
    }
    else
    {
        newStateSlot = mEmptyStateSlotIdx.back();
        mEmptyStateSlotIdx.pop_back();
    } 

    return newStateSlot;
}

void ResourceStateTable::RemoveStateSlot(int stateSlot)
{
    const bool slotExists = stateSlot >= 0 && stateSlot < (int)mStateCounts;
    PG_ASSERT(slotExists);
    if (!slotExists)
        return;

    for (DomainInfo& domain : mDomains)
    {
        PG_ASSERT((unsigned)domain.states.size() == mStateCounts);
        domain.states[stateSlot] = 0;
    }

    mEmptyStateSlotIdx.push_back(stateSlot);
}

void ResourceStateTable::StoreState(Domain d, int stateSlot, uintptr_t state)
{
    const bool domainExists = d.id >= 0 && d.id < (int)mDomains.size();
    const bool stateSlotExists = stateSlot >= 0 && stateSlot < (int)mStateCounts;
    PG_ASSERT(domainExists);
    PG_ASSERT(stateSlotExists);
    if (!domainExists || !stateSlotExists)
        return;
    
    mDomains[d.id].states[stateSlot] = state;
}

bool ResourceStateTable::GetState(Domain d, int stateSlot, uintptr_t& outState)
{
    const bool domainExists = d.id >= 0 && d.id < (int)mDomains.size();
    const bool stateSlotExists = stateSlot >= 0 && stateSlot < (int)mStateCounts;
    if (!domainExists || !stateSlotExists)
        return false;

	auto& inf = mDomains[d.id];
	if (!inf.valid)
		return false;

    outState = inf.states[stateSlot];
    return true;
}

}
}
