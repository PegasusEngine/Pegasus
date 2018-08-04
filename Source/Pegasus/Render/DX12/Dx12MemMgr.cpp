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
}

Dx12MemMgr::~Dx12MemMgr()
{
}

Dx12MemMgr::Handle Dx12MemMgr::AllocateRenderTarget()
{
    return AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
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
