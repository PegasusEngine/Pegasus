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
: mDevice(device), mDesc(desc), mNextBufferIndex(0), mTotalMemory(0), mCurrFrameIndex(0)
{
	CreateNextBuffer(mDesc.initialSize);
	if (desc.usage == Dx12BufferPoolDesc::ConstantBufferUsage)
	{
		mAllocAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	}
	else if (desc.usage == Dx12BufferPoolDesc::BufferUsage)
	{
		mAllocAlignment = 1u;
	}
	else if (desc.usage == Dx12BufferPoolDesc::TextureUsage)
	{
		mAllocAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
	}

    mFrames.resize(mDesc.frameCount);
}

Dx12BufferPool::~Dx12BufferPool()
{
}

UINT Dx12BufferPool::CreateNextBuffer(UINT64 allocation)
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
	resourceDesc.Width = allocation;
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
    return bufferSlotIdx;
}

void Dx12BufferPool::NextFrame()
{
    mCurrFrameIndex = (mCurrFrameIndex + 1) % mDesc.frameCount;
    for (AllocHandle h : mFrames[mCurrFrameIndex].children)
    {
        auto& allocInfo = mAllocs[h];
        auto& bufferSlot = mBufferSlots[allocInfo.bufferSlot];
        bufferSlot.bufferSizeLeft += allocInfo.alignedSize;
        allocInfo.valid = false;
        mFreeAllocs.push_back(h);
    }
	mFrames[mCurrFrameIndex].children.clear();
    mFrames[mCurrFrameIndex].memUsed = 0;
}

Dx12BufferPool::AllocHandle Dx12BufferPool::Allocate(UINT64 size)
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
	}

	UINT64 alignedSize = AlignByte(size, mAllocAlignment);
	auto& newAllocInfo = mAllocs[(UINT)newHandle];
	PG_ASSERT(newAllocInfo.valid == false);
	newAllocInfo.valid = true;
	newAllocInfo.parentFrame = mCurrFrameIndex;

    auto tryAlloc = [&](UINT slotId)
    {
        BufferSlot& currSlot = mBufferSlots[slotId];
        if (currSlot.resource == nullptr)
            return false;

		UINT64 padding = 0u;
        if (alignedSize <= currSlot.bufferSizeLeft)
        {
            UINT64 bytesLeftToEnd = currSlot.bufferSize - currSlot.curr;
            if (alignedSize > bytesLeftToEnd)
            {
                currSlot.curr = (currSlot.curr + bytesLeftToEnd) % currSlot.bufferSize;
                currSlot.bufferSizeLeft -= bytesLeftToEnd;
				padding = bytesLeftToEnd;
            }
        }

        if (alignedSize <= currSlot.bufferSizeLeft)
        {
            newAllocInfo.parentFrame = mCurrFrameIndex;
			newAllocInfo.bufferSlot = slotId;
            newAllocInfo.offset = currSlot.curr;
            newAllocInfo.size = size + padding;
			newAllocInfo.alignedSize = alignedSize + padding;
            currSlot.curr = (currSlot.curr + alignedSize) % currSlot.bufferSize;
            currSlot.bufferSizeLeft -= alignedSize;
            mFrames[mCurrFrameIndex].memUsed += alignedSize;
            return true;
        }

        return false;
    };

    bool allocated = false;
    for (UINT slotId = 0; slotId < mBufferSlots.size() - 1; ++slotId)
    {
        if (tryAlloc(slotId))
        {
            allocated = true;
            break;
        }
    }


    if (!allocated)
    {
		UINT64 nextSuggestedSize = (UINT)((1.0f + (float)mNextBufferIndex * mDesc.growthFactor) * mDesc.initialSize + 0.5);
        if (nextSuggestedSize <= size)
        {
            nextSuggestedSize += (UINT)((1.0f + mDesc.growthFactor)*size + 0.5f); //heuristic ?
        }

        UINT newSlot = CreateNextBuffer(nextSuggestedSize);
        bool allocSuccess = tryAlloc(newSlot);
        PG_ASSERTSTR(allocSuccess, "Allocation did not succeed? this is no good!");
    }
    
    mFrames[mCurrFrameIndex].children.push_back(newHandle);
	return newHandle;
}

void* Dx12BufferPool::GetMem(const Dx12BufferPool::AllocHandle& subHandle)
{
	return nullptr;
}

}
}
