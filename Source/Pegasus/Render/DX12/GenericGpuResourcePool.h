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
	u64 offset = 0ull;
	ID3D12Resource* buffer = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS gpuVA = 0;
};

class GpuUploadPool
{
public:
	GpuUploadPool(Dx12Device* device, ID3D12CommandQueue* queue, u64 initialPoolSize);
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
	u32 descriptorCounts = 0u;
	u32 descriptorSize = 0u;

	bool IsValid() const
	{
		return cpuHandle.ptr != 0;
	}

	bool IsShaderVisible() const
	{
		return gpuHandle.ptr != 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(u32 index = 0) const
	{
		FX_ASSERT(index < descriptorCounts);
		D3D12_CPU_DESCRIPTOR_HANDLE result = { cpuHandle.ptr + index * descriptorSize };
		return result;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(u32 index = 0) const
	{
		FX_ASSERT(index < descriptorCounts);
		D3D12_GPU_DESCRIPTOR_HANDLE result = { gpuHandle.ptr + index * descriptorSize };
		return result;
	}

	void Advance(u32 count)
	{
		FX_ASSERT(count < descriptorCounts);
		cpuHandle = GetCpuHandle(count);
		gpuHandle = GetGpuHandle(count);
		descriptorCounts -= count;
	}

};

class GpuDescriptorTablePool
{
public:
	GpuDescriptorTablePool(Dx12Device* device, ID3D12CommandQueue* queue, u32 maxDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
	~GpuDescriptorTablePool();
	void BeginUsage();
	void EndUsage();
	DescriptorTable AllocateTable(u32 tableSize, ID3D12GraphicsCommandList4* commandList);
	const DescriptorTable& LastAllocatedTable() const { return m_lastTable; }

private:
	class GpuDescriptorTablePoolImpl* m_impl;
	DescriptorTable m_lastTable;

};

}
}


