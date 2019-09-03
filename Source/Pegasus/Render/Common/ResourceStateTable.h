/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ResourceStateTable.h
//! \author Kleber Garcia
//! \date   September 3rd 2019
//! \brief  Resource state tracker for resources in a queue

#pragma once

#include <vector>

namespace Pegasus
{
namespace Render
{

class ResourceStateTable
{
public:
    struct Domain
    {
        int id = -1;
    };

    ResourceStateTable();
    ~ResourceStateTable() {}

    Domain CreateDomain();
    void RemoveDomain(Domain d);

    int CreateStateSlot();
    void RemoveStateSlot(int stateSlot);

    void StoreState(Domain d, int stateSlot, uintptr_t state);
    bool GetState(Domain d, int stateSlot, uintptr_t& outState);

private: 
    struct DomainInfo
    {
        bool valid = false;
        std::vector<uintptr_t> states;
    }; 

    unsigned mStateCounts;
    std::vector<DomainInfo> mDomains;
    std::vector<int> mEmptyDomains;
    std::vector<int> mEmptyStateSlotIdx;
};

}
}
