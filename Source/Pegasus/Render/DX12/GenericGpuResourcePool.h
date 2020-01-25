#pragma once

#include "Dx12Device.h"
#include <d3d12.h>
#include <queue>

namespace Pegasus
{
namespace Render
{

struct GpuMemoryBlock
{
	void* mappedBuffer = nullptr;
	uint64_t offset = 0ull;
	ID3D12Resource* buffer = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS gpuVA = 0;
};

class GpuUploadPool
{
public:
	GpuUploadPool(Dx12Device* device, ID3D12CommandQueue* queue, uint64_t initialPoolSize);
	~GpuUploadPool();
	void BeginUsage();
	void EndUsage();

	GpuMemoryBlock AllocUploadBlock(size_t sizeBytes);

private:
	class GpuUploadPoolImpl* m_impl;
};

struct DescriptorTable
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	ID3D12DescriptorHeap* ownerHeap = nullptr;
	uint32_t descriptorCounts = 0u;
	uint32_t descriptorSize = 0u;

	bool IsValid() const
	{
		return cpuHandle.ptr != 0;
	}

	bool IsShaderVisible() const
	{
		return gpuHandle.ptr != 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t index = 0) const
	{
		PG_ASSERT(index < descriptorCounts);
		D3D12_CPU_DESCRIPTOR_HANDLE result = { cpuHandle.ptr + index * descriptorSize };
		return result;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t index = 0) const
	{
		PG_ASSERT(index < descriptorCounts);
		D3D12_GPU_DESCRIPTOR_HANDLE result = { gpuHandle.ptr + index * descriptorSize };
		return result;
	}

	void Advance(uint32_t count)
	{
		PG_ASSERT(count < descriptorCounts);
		cpuHandle = GetCpuHandle(count);
		gpuHandle = GetGpuHandle(count);
		descriptorCounts -= count;
	}

};

class GpuDescriptorTablePool
{
public:
	GpuDescriptorTablePool(Dx12Device* device, ID3D12CommandQueue* queue, uint32_t maxDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
	~GpuDescriptorTablePool();
	void BeginUsage();
	void EndUsage();
	DescriptorTable AllocateTable(uint32_t tableSize, ID3D12GraphicsCommandList4* commandList);
	const DescriptorTable& LastAllocatedTable() const { return m_lastTable; }

private:
	class GpuDescriptorTablePoolImpl* m_impl;
	DescriptorTable m_lastTable;

};

}
}


