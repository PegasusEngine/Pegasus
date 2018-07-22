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
    Utils::Memset32(&mDHeapsPoolSz, 0, sizeof(mDHeapsPoolSz));
    Utils::Memset32(&mDHeapsIncrSz, 0, sizeof(mDHeapsIncrSz));
    mDHeapsPoolSz[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = 64;
    mDHeapsPoolSz[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = 64;
    mDHeapsPoolSz[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = 64;
    mDHeapsPoolSz[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = 64;
    for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
    {
        mDHeapsIncrSz[i] = device->GetD3D()->GetDescriptorHandleIncrementSize((D3D12_DESCRIPTOR_HEAP_TYPE)i);
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
	mDHeapsFree[h.type].emplace_back(FreeSpot{ h.heapIdx, h.handle });
}

Dx12MemMgr::Handle Dx12MemMgr::AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    PG_ASSERT(type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    //first, check if theres a free available
    auto& freeList = mDHeapsFree[type];
    if (!freeList.empty())
    {
        Dx12MemMgr::FreeSpot& freeSpot = freeList.back();
        freeList.pop_back();
        PG_ASSERT(!mDHeaps[type].empty());
        auto& heapContainer = mDHeaps[type][freeSpot.heapIndex];
        ++heapContainer.activeAllocs;
        PG_ASSERT(heapContainer.activeAllocs <= heapContainer.desc.NumDescriptors);
		return Dx12MemMgr::Handle{ type, freeSpot.handle, freeSpot.heapIndex };
    }
    else
    {
        auto& heapList = mDHeaps[type];
        UINT newHeapIdx = (UINT)heapList.size();
        heapList.emplace_back();
        HeapContainer& newContainer = heapList.back();
        newContainer.desc.Type = type;
        newContainer.desc.NumDescriptors = mDHeapsPoolSz[type];
        newContainer.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        newContainer.desc.NodeMask = 0;
        DX_VALID_DECLARE(mDevice->GetD3D()->CreateDescriptorHeap(&newContainer.desc, __uuidof(newContainer.heap), reinterpret_cast<void**>(&newContainer.heap)));
        Dx12MemMgr::Handle newHandle = { type, newContainer.heap->GetCPUDescriptorHandleForHeapStart(), newHeapIdx};
        //register free spots

        for (UINT i = 1; i < mDHeapsPoolSz[type]; ++i)
        {
            UINT invIdx = mDHeapsPoolSz[type] - i - 1u;
            D3D12_CPU_DESCRIPTOR_HANDLE newFreeHandle =  { newHandle.handle.ptr + invIdx * mDHeapsIncrSz[type] };
			mDHeapsFree[type].emplace_back(FreeSpot{ newHeapIdx, newFreeHandle });
        }

		return newHandle;
    }
}

}
}
