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
        UINT index;
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
        UINT incrSize = 0;
        UINT lastIndex = 0;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        CComPtr<ID3D12DescriptorHeap> heap;
        std::vector<UINT> freeSpots;
    };

    typedef std::vector<HeapContainer> HeapList;

    HeapContainer mHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

};

}
}


