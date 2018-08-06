/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Device.cpp
//! \author Kleber Garcia
//! \date   17th of July 2014
//! \brief  Class that implements the platform specific / render api specific device

#if PEGASUS_GAPI_DX12

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")

#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include "Dx12MemMgr.h"
#include <dxgi1_6.h>
#include <atlbase.h>
#include <Pegasus/Allocator/IAllocator.h>
#include <vector>
#include <string>

using namespace std;

namespace Pegasus
{
namespace Render
{

int Dx12Device::sDeviceRefCounts = 0;
GraphicsCardInfos* Dx12Device::sCardInfos = nullptr;

Dx12Device::Dx12Device(const DeviceConfig& config, Alloc::IAllocator * allocator)
	: IDevice(config, allocator), mDevice(nullptr), mAllocator(allocator)
{
    mIOManager = config.mIOManager;

	if (sDeviceRefCounts == 0)
	{
		PG_ASSERT(sCardInfos == nullptr);
		UINT factoryFlags = 0;
#if PEGASUS_GPU_DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG; //enable debug flag on debug builds.
#endif
		sCardInfos = new GraphicsCardInfos();
		DX_VALID_DECLARE(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&sCardInfos->dxgiFactory)));
	}


	if (sCardInfos && sDeviceRefCounts == 0)
	{
		sCardInfos->cardCounts = 0;
		{
			CComPtr<IDXGIAdapter1> dxgiAdapter;
			for (; sCardInfos->dxgiFactory->EnumAdapters1((UINT)sCardInfos->cardCounts, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++sCardInfos->cardCounts)
				dxgiAdapter = nullptr;
		}

		sCardInfos->infos = new GraphicsCardInfos::Card[sCardInfos->cardCounts];
		for (UINT i = 0; i < (UINT)sCardInfos->cardCounts; ++i)
		{
			CComPtr<IDXGIAdapter1> dxgiAdapter;
			auto ret = sCardInfos->dxgiFactory->EnumAdapters1(i, &dxgiAdapter);
			PG_ASSERT(ret != DXGI_ERROR_NOT_FOUND);
			DX_VALID_DECLARE(dxgiAdapter.QueryInterface(&sCardInfos->infos[i].adapter4));
			DXGI_ADAPTER_DESC1 desc;
			DX_VALID(sCardInfos->infos[i].adapter4->GetDesc1(&desc));
			std::wstring wstr = desc.Description;
			sCardInfos->infos[i].description = string(wstr.begin(), wstr.end());
			PG_LOG('APPL', "found adapter \"%s\"", sCardInfos->infos[i].description.c_str());
		}
	}

#if PEGASUS_DEBUG
	{
		ID3D12Debug* debugInterface;
		DX_VALID_DECLARE(D3D12GetDebugInterface(__uuidof(ID3D12Debug), &((void*)debugInterface)));
		debugInterface->EnableDebugLayer();
		debugInterface->Release();
	}
#endif

	{
		auto* selectedCard = &sCardInfos->infos[sCardInfos->usedIndex];
		PG_LOG('APPL', "Creating device using adapter \"%s\"", selectedCard->description.c_str());

		DX_VALID_DECLARE(D3D12CreateDevice(
			selectedCard->adapter4,
			D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device2), &((void*)mDevice)));
	}

    mQueueManager = D12_NEW(allocator, "dx12QueueManager") Dx12QueueManager(allocator, this);
    mMemMgr = D12_NEW(allocator, "dx12MemMgr") Dx12MemMgr(this);
    ++sDeviceRefCounts;
}

Dx12Device::~Dx12Device()
{
    D12_DELETE(mAllocator, mMemMgr);
    D12_DELETE(mAllocator, mQueueManager);
    --sDeviceRefCounts;

    if (mDevice)
    {
        mDevice->Release();
    }

    if (sDeviceRefCounts == 0 && sCardInfos)
    {
        for (int i = 0; i < sCardInfos->cardCounts; ++i)
        {
            auto& info = sCardInfos->infos[i];
            info.adapter4->Release();
        }
        delete [] sCardInfos->infos;
        sCardInfos->dxgiFactory->Release();
        delete sCardInfos;
        sCardInfos = nullptr;
    }
}

//! platform implementation of device
IDevice * IDevice::CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
{
    return D12_NEW(allocator, "Dx12Device") Dx12Device(config, allocator);
}

}//namespace Render
}//namespace Pegasus

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
