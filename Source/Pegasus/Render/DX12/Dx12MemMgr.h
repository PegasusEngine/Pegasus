/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12MemMgr.h
//! \author Kleber Garcia
//! \date   July 20th 2018
//! \brief  Heaps / allocations etc mem mgr

#include <vector>
#include <d3d12.h>
#include <atlbase.h>

#pragma once

namespace Pegasus
{
namespace Render
{

class Dx12Device;

class Dx12MemMgr
{
public:
    struct Handle
    {
        D3D12_DESCRIPTOR_HEAP_TYPE type;
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        UINT heapIdx;
    };

    Dx12MemMgr(Dx12Device* device);
    ~Dx12MemMgr();
    
    Handle AllocateRenderTarget();
    void Delete(Handle h);

private:

    Handle AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE type);

    Dx12Device* mDevice;

    struct HeapContainer
    {
        UINT activeAllocs = 0;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        CComPtr<ID3D12DescriptorHeap> heap;
    };

    struct FreeSpot
    {
        UINT heapIndex;
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
    };
   
    typedef std::vector<HeapContainer> HeapList;
    typedef std::vector<FreeSpot> FreeList;

    UINT mDHeapsIncrSz[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    UINT mDHeapsPoolSz[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    HeapList mDHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    FreeList mDHeapsFree[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

};

}
}


