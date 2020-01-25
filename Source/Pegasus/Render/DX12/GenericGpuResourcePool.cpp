#include "GenericGpuResourcePool.h"
#include "Dx12Fence.h"
#include "Dx12Defs.h"
#include <algorithm>
#include <queue>

namespace Pegasus
{
namespace Render
{

template<class AllocDesc, class AllocationHandle, class HeapType, class GpuAllocatorType>
class GenericResourcePool
{
public:
	struct Desc
	{
		ID3D12Device* device = nullptr;
		uint64_t initialFenceVal = 0ull;
	};

	GenericResourcePool(Dx12Device* device, ID3D12CommandQueue* queue, GpuAllocatorType* allocator)
	: m_device(device)
	, m_fence(D12_NEW(device->GetAllocator(), "Dx12Fence") Dx12Fence(device, queue))
	, m_nextFenceVal(0ull)
	, m_allocator(allocator)
	{
		PG_ASSERT(m_device != nullptr);
	}

	~GenericResourcePool()
	{
		for (auto& slot : m_heaps)
		{
			if (!slot.ranges.empty())
				m_fence->WaitOnCpu(slot.ranges.front().fenceValue);

			m_allocator->DestroyHeap(slot.heap);
		}
		D12_DELETE(m_device->GetAllocator(), m_fence);
	}

	void BeginUsage()
	{
		m_nextFenceVal++;
	}

	void EndUsage()
	{
		uint64_t currFenceValue = m_fence->GetValue();
		for (HeapSlot& slot : m_heaps)
		{
			while (!slot.ranges.empty())
			{
                if (slot.ranges.front().fenceValue >= m_fence->GetValue())
                    break;

				slot.capacity += slot.ranges.front().size;
				slot.ranges.pop();
				if (slot.ranges.empty())
				{
					PG_ASSERT(slot.capacity == slot.size);
					slot.offset = 0ull;
				}
			}
		}
		m_fence->Signal(m_nextFenceVal);
	}

	AllocationHandle Allocate(const AllocDesc& desc)
	{
		AllocationHandle h;
		if (!InternalFindAlloc(desc, h))
			InternalCreateNew(desc, h);
		return h;
	}

protected:
	struct Range
	{
		uint64_t fenceValue = 0ull;
		uint64_t offset = 0ull;
		uint64_t size = 0ull;
	};

	struct HeapSlot
	{
		std::queue<Range> ranges;
		uint64_t capacity = 0u;
		uint64_t size = 0ull;
		uint64_t offset = 0ull;
		HeapType heap;
	};

	void CommmitRange(HeapSlot& slot, const Range& range)
	{
		if (slot.ranges.empty() || slot.ranges.back().fenceValue < m_nextFenceVal)
			slot.ranges.push(range);
		else
		{
			slot.ranges.back().size += range.size;
		}

		PG_ASSERT(range.size <= slot.capacity);
		slot.capacity -= range.size;
		slot.offset = (slot.offset + range.size) % slot.size;
	}

	void InternalCreateNew(const AllocDesc& desc, AllocationHandle& outHandle)
	{
		m_heaps.emplace_back();
		HeapSlot& newSlot = m_heaps.back();
		newSlot.heap = m_allocator->CreateNewHeap(desc, newSlot.size);
		newSlot.capacity = newSlot.size;
		Range range = {};

		bool rangeResult = CalculateRange(desc, newSlot, range);
		(void)rangeResult;
		PG_ASSERTSTR(rangeResult, "Range result must not fail");

		outHandle = m_allocator->AllocateHandle(desc, range.offset, newSlot.heap);
		CommmitRange(newSlot, range);
	}

	bool CalculateRange(const AllocDesc& desc, const HeapSlot& slot, Range& outRange)
	{
		outRange = {};
		outRange.fenceValue = m_nextFenceVal;
		m_allocator->GetRange(desc, slot.offset, outRange.offset, outRange.size);
		if (outRange.offset >= slot.size)
			return false;

		PG_ASSERT(outRange.offset >= slot.offset);
		uint64_t padding = outRange.offset - slot.offset;
		uint64_t sizeLeft = slot.size - outRange.offset;

		if (outRange.size > sizeLeft)
		{
			padding += sizeLeft;
			outRange.offset = 0ull;
		}

		if ((outRange.size + padding) > slot.capacity)
			return false;

		outRange.size += padding;

		return true;
	}

	bool InternalFindAlloc(const AllocDesc& desc, AllocationHandle& outHandle)
	{
		for (HeapSlot& slot : m_heaps)
		{
			if (slot.capacity == 0u)
				continue;

			Range range = {};
			if (CalculateRange(desc, slot, range))
			{
				outHandle = m_allocator->AllocateHandle(desc, range.offset, slot.heap);
				CommmitRange(slot, range);
				return true;
			}
		}
		return false;
	}

	std::vector<HeapSlot> m_heaps;
	GpuAllocatorType* m_allocator;

	uint64_t m_nextFenceVal;
    Dx12Device* m_device;
	Dx12Fence* m_fence;
};

struct UploadDescDX12
{
	uint64_t alignment = 0ull;
	uint64_t requestBytes = 0ull;
};

struct UploadHeapDX12
{
	ID3D12Heap* heap = nullptr;
	ID3D12Resource* buffer = nullptr;
	void* mappedMemory = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS gpuHeapBaseVA = 0;
	uint64_t size = 0;
};

template<class GpuAllocatorType>
using BaseGpuUploadPool = GenericResourcePool<UploadDescDX12, GpuMemoryBlock, UploadHeapDX12, GpuAllocatorType>;
class GpuUploadPoolImpl : public BaseGpuUploadPool<GpuUploadPoolImpl>
{
public:
	typedef BaseGpuUploadPool<GpuUploadPoolImpl> SuperType;

	GpuUploadPoolImpl(Dx12Device* device, ID3D12CommandQueue* queue, uint64_t initialHeapSize)
	: SuperType(device, queue, this), m_nextHeapSize(initialHeapSize)
	{
	}

	~GpuUploadPoolImpl()
	{
	}

	UploadHeapDX12 CreateNewHeap(const UploadDescDX12& desc, uint64_t& outHeapSize);
	void GetRange(const UploadDescDX12& desc, uint64_t inputOffset, uint64_t& outputOffset, uint64_t& outputSize) const;
	GpuMemoryBlock AllocateHandle(const UploadDescDX12& desc, uint64_t heapOffset, const UploadHeapDX12& heap);
	void DestroyHeap(UploadHeapDX12& heap);
private:
	uint64_t m_nextHeapSize;
};


struct DescriptorTableDescDX12
{
	uint32_t tableSize = 0u;
};

struct DescriptorTableHeapDX12
{
	uint32_t descriptorCounts = 0u;
	ID3D12DescriptorHeap* heap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleStart = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleStart = {};
};

template<class GpuAllocatorType>
using BaseResourceDescriptorPool = GenericResourcePool<DescriptorTableDescDX12, DescriptorTable, DescriptorTableHeapDX12, GpuAllocatorType>;
class GpuDescriptorTablePoolImpl : public BaseResourceDescriptorPool<GpuDescriptorTablePoolImpl>
{
public:
	typedef BaseResourceDescriptorPool<GpuDescriptorTablePoolImpl> SuperType;
	GpuDescriptorTablePoolImpl(Dx12Device* device, ID3D12CommandQueue* queue, uint32_t maxDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType)
	    : SuperType(device, queue, this)
	    , m_maxDescriptorCount(maxDescriptorCount)
	    , m_heapType(heapType)
	{
		m_descriptorSize = m_device->GetD3D()->GetDescriptorHandleIncrementSize(heapType);
	}

	~GpuDescriptorTablePoolImpl()
	{
	}

	DescriptorTableHeapDX12 CreateNewHeap(const DescriptorTableDescDX12& desc, uint64_t& outHeapSize);
	void GetRange(const DescriptorTableDescDX12& desc, uint64_t inputOffset, uint64_t& outputOffset, uint64_t& outputSize) const;
	DescriptorTable AllocateHandle(const DescriptorTableDescDX12& desc, uint64_t heapOffset, const DescriptorTableHeapDX12& heap);
	void DestroyHeap(DescriptorTableHeapDX12& heap);

private:
	uint32_t m_descriptorSize;
	uint32_t m_maxDescriptorCount;
	D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
};


UploadHeapDX12 GpuUploadPoolImpl::CreateNewHeap(const UploadDescDX12& desc, uint64_t& outHeapSize)
{
	UploadHeapDX12 outHeap = {};

	D3D12_HEAP_DESC heapDesc = {};
	heapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	heapDesc.SizeInBytes = max(2*desc.requestBytes, m_nextHeapSize);
	heapDesc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	m_nextHeapSize = max(2*desc.requestBytes, 2 * m_nextHeapSize);
	
#if FLUX_CHECK_MEMORY_USAGE_ON_ALLOCATION
//	DXGI_QUERY_VIDEO_MEMORY_INFO memInfo{};
//	m_dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
//	PG_ASSERT_MSG(memInfo.CurrentUsage + desc.SizeInBytes <= memInfo.Budget,
//		"Current memory budget is %u and current usage is %u, cannot Allocate %u more bytes.",
//		memInfo.Budget, memInfo.CurrentUsage, desc.SizeInBytes);
#endif

	DX_VALID_DECLARE(m_device->GetD3D()->CreateHeap(&heapDesc, IID_PPV_ARGS(&outHeap.heap)));

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = heapDesc.SizeInBytes;
	resourceDesc.Height = resourceDesc.SampleDesc.Count = resourceDesc.DepthOrArraySize = resourceDesc.MipLevels = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	DX_VALID(m_device->GetD3D()->CreatePlacedResource(outHeap.heap, 0, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&outHeap.buffer)));
	outHeap.size = heapDesc.SizeInBytes;
	outHeap.gpuHeapBaseVA = outHeap.buffer->GetGPUVirtualAddress();
	outHeapSize = outHeap.size;
	DX_VALID(outHeap.buffer->Map(0, nullptr, &outHeap.mappedMemory));
	return outHeap;
}

void GpuUploadPoolImpl::GetRange(const UploadDescDX12& desc, uint64_t inputOffset, uint64_t& outOffset, uint64_t& outSize) const
{
	outSize = desc.requestBytes;
	outOffset = AlignByte(inputOffset, desc.alignment);
}

GpuMemoryBlock GpuUploadPoolImpl::AllocateHandle(const UploadDescDX12& desc, uint64_t heapOffset, const UploadHeapDX12& heap)
{
	PG_ASSERT((heapOffset + desc.requestBytes) <= heap.size);
	PG_ASSERT((heapOffset % desc.alignment) == 0u);

	GpuMemoryBlock block = {};
	block.buffer = heap.buffer;
	block.mappedBuffer = static_cast<void*>(static_cast<char*>(heap.mappedMemory) + heapOffset);
	block.gpuVA  = heap.gpuHeapBaseVA + heapOffset;
	block.offset = heapOffset;
	return block;
}

void GpuUploadPoolImpl::DestroyHeap(UploadHeapDX12& heap)
{
	if (heap.heap)
        heap.heap->Release();

    if (heap.buffer)
        heap.buffer->Release();
}


DescriptorTableHeapDX12 GpuDescriptorTablePoolImpl::CreateNewHeap(const DescriptorTableDescDX12& desc, uint64_t& outHeapSize)
{
	uint32_t heapTargetSize = max(m_maxDescriptorCount, desc.tableSize);
	DescriptorTableHeapDX12 outHeap{};
	outHeapSize = heapTargetSize;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NumDescriptors = heapTargetSize;
	heapDesc.Type = m_heapType;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DX_VALID_DECLARE(m_device->GetD3D()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&outHeap.heap)));

	outHeap.cpuHandleStart = outHeap.heap->GetCPUDescriptorHandleForHeapStart();
	outHeap.gpuHandleStart = outHeap.heap->GetGPUDescriptorHandleForHeapStart();
	outHeap.descriptorCounts = heapTargetSize;

	return outHeap;
}

void GpuDescriptorTablePoolImpl::GetRange(const DescriptorTableDescDX12& desc, uint64_t inputOffset, uint64_t& outputOffset, uint64_t& outputSize) const
{ 
	outputOffset = inputOffset; //no alignment requirements
	outputSize = desc.tableSize;
}

DescriptorTable GpuDescriptorTablePoolImpl::AllocateHandle(const DescriptorTableDescDX12& desc, uint64_t heapOffset, const DescriptorTableHeapDX12& heap)
{
	PG_ASSERT(heapOffset < heap.descriptorCounts);
	PG_ASSERT(desc.tableSize <= (heap.descriptorCounts - heapOffset));
	DescriptorTable outTable{};
	outTable.cpuHandle = { heap.cpuHandleStart.ptr + (uint32_t)heapOffset * m_descriptorSize };
	outTable.gpuHandle = { heap.gpuHandleStart.ptr + (uint32_t)heapOffset * m_descriptorSize };
	outTable.ownerHeap = heap.heap;
	outTable.descriptorCounts = desc.tableSize;
	outTable.descriptorSize = m_descriptorSize;
	return outTable;
}

void GpuDescriptorTablePoolImpl::DestroyHeap(DescriptorTableHeapDX12& heap)
{
	if (heap.heap)
        heap.heap->Release();
}

GpuUploadPool::GpuUploadPool(Dx12Device* device, ID3D12CommandQueue* queue, uint64_t initialPoolSize)
{
	m_impl = new GpuUploadPoolImpl(device, queue, initialPoolSize);
}

void GpuUploadPool::BeginUsage()
{
	m_impl->BeginUsage();
}

void GpuUploadPool::EndUsage()
{
	m_impl->EndUsage();
}

GpuUploadPool::~GpuUploadPool()
{
	delete m_impl;
}

GpuMemoryBlock GpuUploadPool::AllocUploadBlock(size_t sizeBytes)
{
	UploadDescDX12 desc;
	desc.alignment = 256ull;
	desc.requestBytes = sizeBytes;
	return m_impl->Allocate(desc);
}

GpuDescriptorTablePool::GpuDescriptorTablePool(Dx12Device* device, ID3D12CommandQueue* queue, uint32_t maxDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType)
{
	m_impl = new GpuDescriptorTablePoolImpl(device, queue, maxDescriptorCount, heapType);
}

GpuDescriptorTablePool::~GpuDescriptorTablePool()
{
	delete m_impl;
}

void GpuDescriptorTablePool::BeginUsage()
{
	m_impl->BeginUsage();
}

void GpuDescriptorTablePool::EndUsage()
{
	m_impl->EndUsage();
}

DescriptorTable GpuDescriptorTablePool::AllocateTable(uint32_t tableSize, ID3D12GraphicsCommandList4* commandList)
{
	PG_ASSERT(tableSize > 0u);
	DescriptorTableDescDX12 desc;
	desc.tableSize = tableSize;
	DescriptorTable result = m_impl->Allocate(desc);
	m_lastTable = result;
	commandList->SetDescriptorHeaps(1u, &result.ownerHeap);
	return result;
}

}
}

