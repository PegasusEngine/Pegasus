/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RenderContext.h
//! \author Kleber Garcia
//! \date   March 9 2019
//! \brief  Buffer pool, used for multi frame allocations / upload heaps / constant buffer holding
//!		This class is unsafe in terms of GPU coordination. No gpu sync is done, so be sure to use
//!		fences externally;

#pragma once

#include <D3D12.h>
#include <atlbase.h>
#include "Dx12Defs.h"
#include <vector>

namespace Pegasus
{
namespace Render
{

class Dx12Device;

struct Dx12BufferPoolDesc
{
	enum Usage 
	{
		ConstantBufferUsage,
		BufferUsage,
		TextureUsage
	};

	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
    UINT frameCount = 0;
	Usage usage = BufferUsage;
	UINT64 initialSize = 1024 * 1024 * 2; //2MB
	float growthFactor = 0.5;//next buffer increments by a factor of (1.0f+N*growthFactor)*initialSize 
	UINT garbageCollectTicks = 20; //number of ticks before largest empty buffer gets destroyed
};

class Dx12BufferPool
{
public:
	struct AllocHandle 
    {
	    UINT index;
	    AllocHandle() { index = 0xffffffff; }
		AllocHandle(INT idx) { index = idx; }
	    bool operator==(const AllocHandle& other) { return other.index == index; }
	    operator UINT() const { return index; }
	    bool isValid() const { return index == 0xffffffff; }
	};

	Dx12BufferPool(Dx12Device* device, const Dx12BufferPoolDesc& desc);
	~Dx12BufferPool();
    UINT CurrFrame() const { return mCurrFrameIndex; }
    void NextFrame();
	AllocHandle Allocate(UINT size);
	void* GetMem(const AllocHandle& subHandle);
	void GarbageCollect();

private:
	UINT CreateNextBuffer(UINT64 allocation);

	struct BufferSlot
	{
		UINT index = 0;
		UINT refs = 0;
		UINT64 bufferSize = 0;
		UINT64 bufferSizeLeft = 0;
		UINT64 curr = 0;
		CComPtr<ID3D12Resource> resource;
		UINT8* mappedMem = nullptr;
	};

	struct AllocInfo
	{
		bool valid = false;
		UINT parentFrame = 0;
        UINT bufferSlot = 0;
        UINT64 offset = 0;
        UINT64 size = 0;
	};
   
    struct FrameContainer
    {
        std::vector<AllocHandle> children;
        UINT memUsed = 0u;
    };

	Dx12Device* mDevice;
	Dx12BufferPoolDesc mDesc;
	std::vector<AllocInfo> mAllocs;
	std::vector<AllocHandle> mFreeAllocs;
	std::vector<BufferSlot> mBufferSlots;
    std::vector<UINT> mFreeBufferSlots;
	UINT64 mAllocAlignment;
	UINT mNextBufferIndex;
    UINT mCurrFrameIndex;
	SIZE_T mTotalMemory;
    std::vector<FrameContainer> mFrames;
};

}
}
