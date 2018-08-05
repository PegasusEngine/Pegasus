/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12MemMgr.cpp
//! \author Kleber Garcia
//! \date   July 20th 2018
//! \brief  Heaps / allocations etc mem mgr

#include "Dx12MemMgr.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include <Pegasus/Utils/Memset.h>

namespace Pegasus
{
namespace Render
{

Dx12MemMgr::Dx12MemMgr(Dx12Device* device)
: mDevice(device)
{
    for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        HeapContainer& container = mHeaps[i];
        auto& desc = container.desc;
        desc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)i;
        desc.NodeMask = 0;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        switch(desc.Type)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            desc.NumDescriptors = 1024;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            desc.NumDescriptors = 1024;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            desc.NumDescriptors = 1024;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            desc.NumDescriptors = 1024;
            break;
        default:
            break;
        }
        container.incrSize = device->GetD3D()->GetDescriptorHandleIncrementSize(desc.Type);
        DX_VALID_DECLARE(device->GetD3D()->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap),reinterpret_cast<void**>(&container.heap)));
    }

    //Create a single Heap for SRV/UAV/Cbv
    mGpuHeap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    mGpuHeap.desc.NumDescriptors = 20000;
    mGpuHeap.desc.NodeMask = 0;
    mGpuHeap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    mGpuHeap.lastIndex = 0;
    mGpuHeap.incrSize = device->GetD3D()->GetDescriptorHandleIncrementSize(mGpuHeap.desc.Type);
    DX_VALID_DECLARE(device->GetD3D()->CreateDescriptorHeap(&mGpuHeap.desc, __uuidof(ID3D12DescriptorHeap),reinterpret_cast<void**>(&mGpuHeap.heap)));
}

Dx12MemMgr::~Dx12MemMgr()
{
}

Dx12MemMgr::Table Dx12MemMgr::AllocEmptyTable(UINT count, Dx12MemMgr::TableType tableType)
{
    PG_ASSERT(count > 0);
    Table resultTable;

    //for now linearly find a heap that fits
    for (int i = 0; i < (int)mGpuHeap.freeSpots.size(); ++i)
    {
        auto& freeTable = mGpuHeap.freeSpots[i];
        if (count == freeTable.count)
        {
            resultTable = freeTable;
            resultTable.tableType = tableType;
            mGpuHeap.freeSpots[i] = mGpuHeap.freeSpots[mGpuHeap.freeSpots.size() - 1];
            mGpuHeap.freeSpots.pop_back();
            return resultTable;
        }
    }

    //if not check if theres enough size
    PG_ASSERTSTR(count <= (mGpuHeap.desc.NumDescriptors - mGpuHeap.lastIndex), "Not enough memory remaining for descriptors!");
    resultTable.tableType = tableType;
    resultTable.count = count;
    resultTable.baseIdx = count;
    resultTable.baseHandle = mGpuHeap.heap->GetCPUDescriptorHandleForHeapStart();
    resultTable.baseHandleGpu = mGpuHeap.heap->GetGPUDescriptorHandleForHeapStart();
    resultTable.baseHandle.ptr += mGpuHeap.incrSize * mGpuHeap.lastIndex;
    resultTable.baseHandleGpu.ptr += mGpuHeap.incrSize * mGpuHeap.lastIndex;
    mGpuHeap.lastIndex += count;
	return resultTable;
}

Dx12MemMgr::Table Dx12MemMgr::AllocateTable(Dx12MemMgr::TableType tableType, const Dx12MemMgr::Handle* cpuHandles, UINT cpuHandlesCount)
{
    Dx12MemMgr::Table resultTable = AllocEmptyTable(cpuHandlesCount, tableType);
    for (UINT i = 0; i < cpuHandlesCount; ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE tableDescriptor = resultTable.baseHandle;
        tableDescriptor.ptr += i * mGpuHeap.incrSize;
        mDevice->GetD3D()->CopyDescriptorsSimple(1, tableDescriptor, cpuHandles[i].handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    return resultTable;
}

Dx12MemMgr::Table Dx12MemMgr::AllocateTableCopy(const Table& srcTable)
{
    Dx12MemMgr::Table resultTable = AllocEmptyTable(srcTable.count, srcTable.tableType);
    mDevice->GetD3D()->CopyDescriptorsSimple(srcTable.count, resultTable.baseHandle, srcTable.baseHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return resultTable;
}

void Dx12MemMgr::Delete(const Dx12MemMgr::Table& t)
{
    mGpuHeap.freeSpots.push_back(t);
}

void Dx12MemMgr::Delete(Dx12MemMgr::Handle h)
{
    PG_ASSERT(h.type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    auto& container = mHeaps[h.type];
    container.freeSpots.push_back(h.index);
}

Dx12MemMgr::Handle Dx12MemMgr::AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    PG_ASSERT(type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    auto& container = mHeaps[type];
    auto& freeList = container.freeSpots;
    auto& desc = container.desc;
    UINT cpuRequestedIndex = 0;
    if (freeList.empty())
    {
        if (container.lastIndex >= desc.NumDescriptors)
            PG_FAILSTR("Not enough dx12 descriptors!");
        cpuRequestedIndex = container.lastIndex++;
    }
    else
    {
        cpuRequestedIndex = freeList.back();
        freeList.pop_back();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = container.heap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += container.incrSize * cpuRequestedIndex;
    return Dx12MemMgr::Handle { desc.Type, cpuHandle, cpuRequestedIndex };
}

}
}
