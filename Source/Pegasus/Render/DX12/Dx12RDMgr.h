/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RDMgr.h
//! \author Kleber Garcia
//! \date   July 20th 2018
//! \brief  Resource descriptor manager.

#pragma once

#include <vector>
#include <d3d12.h>
#include <atlbase.h>

namespace Pegasus
{
namespace Render
{

class Dx12Device;
class Dx12Fence;

class Dx12RDMgr
{
public:

    struct Handle
    {
        D3D12_DESCRIPTOR_HEAP_TYPE type;
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        UINT index;
    };

    enum TableType
    {
        TableTypeSrvCbvUav, TableTypeSampler, TableTypeRtv, TableTypeMax, TableTypeInvalid
    };

    static inline TableType GetTableType(D3D12_DESCRIPTOR_HEAP_TYPE heapType) 
    {
        switch(heapType)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            return TableTypeSrvCbvUav;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            return TableTypeSampler;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            return TableTypeRtv;
        }
        return TableTypeInvalid;
    }

    static inline D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType(TableType tt)
    {
        switch(tt)
        {
        case TableTypeSrvCbvUav:
            return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        case TableTypeSampler:
            return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        case TableTypeRtv:
            return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        }
        return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    }

    struct Table
    {
        D3D12_CPU_DESCRIPTOR_HANDLE baseHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE baseHandleGpu;
        TableType tableType = TableTypeSrvCbvUav;
        UINT baseIdx = 0;   
        UINT count = 0;
    };

    Dx12RDMgr(Dx12Device* device);
    ~Dx12RDMgr();
    
    inline Handle AllocateRenderTarget() { return AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }
    inline Handle AllocateSrvOrUavOrCbv() { return AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }

    Table AllocateTable(TableType tableType, const Handle* cpuHandles, UINT cpuHandleCounts);
    Table AllocateTableCopy(const Table& srcTable);

    void Delete(const Table& t);
    void Delete(Handle h);

private:
    Handle AllocInternal(D3D12_DESCRIPTOR_HEAP_TYPE type);
    Table AllocEmptyTable(UINT count, TableType tableType);

    Dx12Device* mDevice;

    struct HeapContainer
    {
        UINT incrSize = 0;
        UINT lastIndex = 0;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        CComPtr<ID3D12DescriptorHeap> heap;
        std::vector<UINT> freeSpots;
    };

    struct TableHeapContainer
    {
        UINT incrSize = 0;
        UINT lastIndex = 0;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        CComPtr<ID3D12DescriptorHeap> heap;
        std::vector<Table> freeSpots;

        D3D12_DESCRIPTOR_HEAP_DESC gpuLinearHeapDesc;
        CComPtr<ID3D12DescriptorHeap> gpuLinearHeap;
    };

    HeapContainer mHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	TableHeapContainer mTableHeaps[TableTypeMax];
};

}
}


