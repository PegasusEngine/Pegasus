/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RenderContext.cpp
//! \author Kleber Garcia
//! \date   March 9 2019
//! \brief  Buffer pool, used for multi frame allocations / upload heaps / constant buffer holding
//!		This class is unsafe in terms of GPU coordination. No gpu sync is done, so be sure to use
//!		fences externally;

#include "Dx12BufferPool.h"
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

Dx12BufferPool::Dx12BufferPool(Dx12Device* device, const Dx12BufferPoolDesc& desc)
: mDevice(device), mDesc(desc), mNextBufferIndex(0), mTotalMemory(0)
{
	CreateNextBuffer();
	if (desc.usage == Dx12BufferPoolDesc::ConstantBufferUsage)
	{
		mAllocAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	}
	else if (desc.usage == Dx12BufferPoolDesc::BufferUsage)
	{
		mAllocAlignment = 0u;
	}
	else if (desc.usage == Dx12BufferPoolDesc::TextureUsage)
	{
		mAllocAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
	}
}

Dx12BufferPool::~Dx12BufferPool()
{
}

void Dx12BufferPool::CreateNextBuffer()
{
	D3D12_HEAP_PROPERTIES heapProps;
	heapProps.Type = mDesc.heapType;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 0u;
	heapProps.VisibleNodeMask = 0u;
	D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0u;
	resourceDesc.Width = (UINT)((1.0f + (float)mNextBufferIndex * mDesc.growthFactor) * mDesc.initialSize + 0.5);
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	UINT bufferSlotIdx = 0;
	if (mFreeBufferSlots.empty())
	{
		bufferSlotIdx = (UINT)mBufferSlots.size();
		mBufferSlots.emplace_back();
	}
	else
	{
		bufferSlotIdx = mFreeBufferSlots.back();
		mFreeBufferSlots.pop_back();
	}

	auto& newSlot = mBufferSlots[bufferSlotIdx];
	PG_ASSERT(newSlot.resource == nullptr);
	DX_VALID_DECLARE(mDevice->GetD3D()->CreateCommittedResource(
		&heapProps,
		heapFlags,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		__uuidof(ID3D12Resource),
		reinterpret_cast<void**>(&newSlot.resource)
	));
	newSlot.index = mNextBufferIndex;
	newSlot.refs = 0;
	if (mDesc.heapType == D3D12_HEAP_TYPE_UPLOAD && newSlot.resource != nullptr)
	{
		D3D12_RANGE readRange = { 0, 0 };
		void* pData = nullptr;
		DX_VALID(newSlot.resource->Map(
			0, &readRange, &pData
		));
		newSlot.mappedMem = reinterpret_cast<UINT8*>(pData);
		PG_ASSERT(newSlot.mappedMem != nullptr);
	}
	newSlot.bufferSizeLeft = newSlot.bufferSize = resourceDesc.Width;

	++mNextBufferIndex;
}

Dx12BufferPool::AllocHandle Dx12BufferPool::Create()
{
	AllocHandle newHandle;
	if (mFreeAllocs.empty())
	{
		newHandle = AllocHandle((UINT)mAllocs.size());
		mAllocs.emplace_back();
	}
	else
	{
		newHandle = mFreeAllocs.back();
		mFreeAllocs.pop_back();
		PG_ASSERT((UINT)newHandle < (UINT)mAllocs.size());
	}

	auto& newInfo = mAllocs[(UINT)newHandle];
	PG_ASSERT(newInfo.valid == false);
	newInfo.valid = true;
	return newHandle;
}

void Dx12BufferPool::Reset(const Dx12BufferPool::AllocHandle& handle)
{
	PG_ASSERT(handle.isValid());
	PG_ASSERT((UINT)handle < (UINT)mAllocs.size());
	auto& oldAlloc = mAllocs[(UINT)handle];
	for (auto& subAllocHandle : oldAlloc.children)
	{
		auto& oldChild = mSubAllocs[(UINT)subAllocHandle];
		PG_ASSERT(oldChild.parent == handle);
		PG_ASSERT(oldChild.valid == true);
		oldChild.valid = false;
		mFreeSubAllocs.push_back(subAllocHandle);
	}
	oldAlloc.children.clear();
}

void Dx12BufferPool::Delete(const Dx12BufferPool::AllocHandle& handle)
{
	PG_ASSERT(handle.isValid());
	PG_ASSERT((UINT)handle < (UINT)mAllocs.size());
	Reset(handle);
	auto& oldAlloc = mAllocs[(UINT)handle];
	oldAlloc.valid = false;
	mFreeAllocs.push_back(handle);
}

Dx12BufferPool::SubAllocHandle Dx12BufferPool::SubAllocate(const AllocHandle& handle, UINT size)
{
	PG_ASSERT(handle.isValid());
	PG_ASSERT((UINT)handle < (UINT)mAllocs.size());
	auto& allocInfo = mAllocs[(UINT)handle];

	SubAllocHandle newHandle;
	if (mFreeSubAllocs.empty())
	{
		newHandle = SubAllocHandle((UINT)mSubAllocs.size());
		mSubAllocs.emplace_back();
	}
	else
	{
		newHandle = mFreeSubAllocs.back();
		mFreeSubAllocs.pop_back();
	}

	auto& newSubAllocInfo = mSubAllocs[(UINT)newHandle];
	PG_ASSERT(newSubAllocInfo.valid == false);
	newSubAllocInfo.valid = true;
	newSubAllocInfo.parent = handle;
	return newHandle;
}

void* Dx12BufferPool::GetMem(const Dx12BufferPool::SubAllocHandle& subHandle)
{
	return nullptr;
}

}
}
