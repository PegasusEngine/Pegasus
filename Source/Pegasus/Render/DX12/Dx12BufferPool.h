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

template<typename T>
struct Dx12BufferPoolGenericHandle
{
	T index;
	Dx12BufferPoolGenericHandle() { index = 0xffffffff; }
	Dx12BufferPoolGenericHandle(T idx) { index = idx; }
	bool operator==(const Dx12BufferPoolGenericHandle& other) { return other.index == index; }
	operator T() const { return index; }
	bool isValid() const { return index == 0xffffffff; }
};

struct Dx12BufferPoolDesc
{
	enum Usage 
	{
		ConstantBufferUsage,
		BufferUsage,
		TextureUsage
	};

	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
	Usage usage = BufferUsage;
	UINT64 initialSize = 1024 * 1024 * 2; //2MB
	float growthFactor = 0.5;//next buffer increments by a factor of (1.0f+N*growthFactor)*initialSize 
	UINT garbageCollectTicks = 20; //number of ticks before largest empty buffer gets destroyed
};

class Dx12BufferPool
{
public:
	struct AllocHandle : Dx12BufferPoolGenericHandle<UINT> {
		AllocHandle(){} 
		AllocHandle(UINT index) : Dx12BufferPoolGenericHandle<UINT>(index) {}
	};

	struct SubAllocHandle : Dx12BufferPoolGenericHandle<UINT> { 
		SubAllocHandle(){}
		SubAllocHandle(UINT index) : Dx12BufferPoolGenericHandle<UINT>(index) {}
	};

	Dx12BufferPool(Dx12Device* device, const Dx12BufferPoolDesc& desc);
	~Dx12BufferPool();
	AllocHandle Create();
	void Reset(const AllocHandle& handle);
	void Delete(const AllocHandle& handle);
	void GarbageCollect();

	SubAllocHandle SubAllocate(const AllocHandle& handle, UINT size);
	void* GetMem(const SubAllocHandle& subHandle);

private:
	void CreateNextBuffer();

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

	struct SubAllocInfo
	{
		bool valid = false;
		AllocHandle parent;
	};

	struct AllocInfo
	{
		bool valid = false;
		std::vector<SubAllocHandle> children;
	};

	Dx12Device* mDevice;
	Dx12BufferPoolDesc mDesc;
	std::vector<AllocInfo> mAllocs;
	std::vector<SubAllocInfo> mSubAllocs;
	std::vector<AllocHandle> mFreeAllocs;
	std::vector<SubAllocHandle> mFreeSubAllocs;
	std::vector<BufferSlot> mBufferSlots;
	std::vector<UINT> mFreeBufferSlots;
	UINT64 mAllocAlignment;
	UINT mNextBufferIndex;
	SIZE_T mTotalMemory;
};

}
}
