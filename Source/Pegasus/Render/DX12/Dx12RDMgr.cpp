/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RDMgr.cpp
//! \author Kleber Garcia
//! \date   July 20th 2018
//! \brief  Resource descriptor manager

#include "Dx12RDMgr.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12Fence.h"
#include "Dx12QueueManager.h"
#include <Pegasus/Utils/Memset.h>


namespace Pegasus
{
namespace Render
{

Dx12RDMgr::Dx12RDMgr(Dx12Device* device)
: mDevice(device), mNextTableGuid(1)
{
    for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        HeapContainer& container = mHeaps[i];
        auto& desc = container.desc;
        desc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)i;
        desc.NodeMask = 0;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        UINT tableTypeCounts = 0;

        switch(desc.Type)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            desc.NumDescriptors = 1024;
            tableTypeCounts = 20000;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            desc.NumDescriptors = 1024;
            tableTypeCounts = 20000;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            desc.NumDescriptors = 1024;
            tableTypeCounts = 20000;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            desc.NumDescriptors = 1024;
            tableTypeCounts = 0;
            break;
        default:
            break;
        }

        container.incrSize = device->GetD3D()->GetDescriptorHandleIncrementSize(desc.Type);
        DX_VALID_DECLARE(device->GetD3D()->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap),reinterpret_cast<void**>(&container.heap)));
        
        TableType tableType = GetTableType(desc.Type);
        if (tableType != TableTypeInvalid && tableTypeCounts > 0)
        {
			mTableHeaps[tableType].desc = {};
            mTableHeaps[tableType].desc.Type = desc.Type;
            mTableHeaps[tableType].desc.NumDescriptors = tableTypeCounts;
            mTableHeaps[tableType].desc.NodeMask = 0;
            mTableHeaps[tableType].lastIndex = 0;
            mTableHeaps[tableType].incrSize = device->GetD3D()->GetDescriptorHandleIncrementSize(mTableHeaps[tableType].desc.Type);
            DX_VALID_DECLARE(device->GetD3D()->CreateDescriptorHeap(
                &mTableHeaps[tableType].desc,
                __uuidof(ID3D12DescriptorHeap),
                reinterpret_cast<void**>(&mTableHeaps[tableType].heap)));
        }
    }
}

Dx12RDMgr::~Dx12RDMgr()
{
}

Dx12RDMgr::Table Dx12RDMgr::AllocEmptyTable(UINT count, Dx12RDMgr::TableType tableType)
{
    PG_ASSERT(count > 0);
    Table resultTable;

    //for now linearly find a heap that fits
    for (int i = 0; i < (int)mTableHeaps[tableType].freeSpots.size(); ++i)
    {
        auto& freeTable = mTableHeaps[tableType].freeSpots[i];
        if (count == freeTable.count)
        {
            resultTable = freeTable;
            resultTable.tableType = tableType;
            mTableHeaps[tableType].freeSpots[i] = mTableHeaps[tableType].freeSpots[mTableHeaps[tableType].freeSpots.size() - 1];
            mTableHeaps[tableType].freeSpots.pop_back();
            return resultTable;
        }
    }

    //if not check if theres enough size
    PG_ASSERTSTR(count <= (mTableHeaps[tableType].desc.NumDescriptors - mTableHeaps[tableType].lastIndex), "Not enough memory remaining for descriptors!");
    resultTable.tableType = tableType;
    resultTable.count = count;
    resultTable.baseHandle = mTableHeaps[tableType].heap->GetCPUDescriptorHandleForHeapStart();
    resultTable.baseHandle.ptr += mTableHeaps[tableType].incrSize * mTableHeaps[tableType].lastIndex;
    resultTable.guid = mNextTableGuid++;
    mTableHeaps[tableType].lastIndex += count;
    return resultTable;
}

Dx12RDMgr::Table Dx12RDMgr::AllocateTable(Dx12RDMgr::TableType tableType, const Dx12RDMgr::Handle* cpuHandles, UINT cpuHandlesCount)
{
    Dx12RDMgr::Table resultTable = AllocEmptyTable(cpuHandlesCount, tableType);
    for (UINT i = 0; i < cpuHandlesCount; ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE tableDescriptor = resultTable.baseHandle;
        tableDescriptor.ptr += i * mTableHeaps[tableType].incrSize;
        mDevice->GetD3D()->CopyDescriptorsSimple(1, tableDescriptor, cpuHandles[i].handle, GetHeapType(tableType));
    }
    return resultTable;
}

Dx12RDMgr::Table Dx12RDMgr::AllocateTableCopy(const Table& srcTable)
{
    Dx12RDMgr::Table resultTable = AllocEmptyTable(srcTable.count, srcTable.tableType);
    mDevice->GetD3D()->CopyDescriptorsSimple(srcTable.count, resultTable.baseHandle, srcTable.baseHandle, GetHeapType(srcTable.tableType));
    return resultTable;
}

void Dx12RDMgr::Delete(const Dx12RDMgr::Table& t)
{
    PG_ASSERT((UINT)t.tableType >= 0 && t.tableType < TableTypeMax);
    mTableHeaps[t.tableType].freeSpots.push_back(t);
}

void Dx12RDMgr::Delete(Dx12RDMgr::Handle h)
{
    PG_ASSERT(h.type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    auto& container = mHeaps[h.type];
    container.freeSpots.push_back(h.index);
}

Dx12RDMgr::Handle Dx12RDMgr::AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    PG_ASSERT(type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    auto& container = mHeaps[type];
    auto& freeList = container.freeSpots;
    auto& desc = container.desc;
    UINT cpuRequestedIndex = 0;
    if (freeList.empty())
    {
#if PEGASUS_DEBUG
        if (container.lastIndex >= desc.NumDescriptors)
            PG_FAILSTR("Not enough dx12 descriptors!");
#endif
        cpuRequestedIndex = container.lastIndex++;
    }
    else
    {
        cpuRequestedIndex = freeList.back();
        freeList.pop_back();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = container.heap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += container.incrSize * cpuRequestedIndex;
    return Dx12RDMgr::Handle { desc.Type, cpuHandle, cpuRequestedIndex };
}

}
}
