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

#include <Pegasus/Allocator/IAllocator.h>
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include "Dx12RDMgr.h"
#include "Dx12Resources.h"
#include "Dx12GpuProgram.h"
#include <dxgi1_6.h>
#include <atlbase.h>
#include <vector>
#include <string>

#if PEGASUS_DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid")
#endif

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
		DX_VALID_DECLARE(D3D12GetDebugInterface(__uuidof(ID3D12Debug), &((void*)mDebugLayer)));
		mDebugLayer->EnableDebugLayer();
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
    mRDMgr = D12_NEW(allocator, "dx12RDMgr") Dx12RDMgr(this);
    ++sDeviceRefCounts;
}

Dx12Device::~Dx12Device()
{
    D12_DELETE(mAllocator, mRDMgr);
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

#if PEGASUS_DEBUG
	{
		IDXGIDebug* dxgiDebugLayer = nullptr;
		HMODULE dxgidebuglib = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (dxgidebuglib)
		{
			typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void **);
			auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
				reinterpret_cast<void*>(GetProcAddress(dxgidebuglib, "DXGIGetDebugInterface")));

			DX_VALID_DECLARE(dxgiGetDebugInterface(__uuidof(IDXGIDebug), &((void*)dxgiDebugLayer)));
			DX_VALID(dxgiDebugLayer->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
			dxgiDebugLayer->Release();
		}
	}
	mDebugLayer->Release();
#endif
}

Dx12GpuProgramRef Dx12Device::CreateGpuProgram()
{
	Dx12GpuProgramRef prog = D12_NEW(mAllocator, "Dx12GpuProgram") Dx12GpuProgram(this);
	return prog;
}

BufferRef Dx12Device::InternalCreateBuffer(const BufferConfig& config)
{
	Dx12BufferRef buff = D12_NEW(mAllocator, "Dx12Buffer") Dx12Buffer(config, this);
	buff->init();
	return buff;
}

TextureRef Dx12Device::InternalCreateTexture(const TextureConfig& config)
{
	Dx12TextureRef texRef =  D12_NEW(mAllocator, "Dx12Texture") Dx12Texture(config, this);
	texRef->init();
	return texRef;
}

RenderTargetRef Dx12Device::InternalCreateRenderTarget(const RenderTargetConfig& config)
{
    Dx12RenderTargetRef rtRef = D12_NEW(mAllocator, "Dx12RenderTarget") Dx12RenderTarget(config, this);
    return nullptr;
}

ResourceTableRef Dx12Device::InternalCreateResourceTable(const ResourceTableConfig& config)
{
	Dx12ResourceTableRef resTable =  D12_NEW(mAllocator, "Dx12Texture") Dx12ResourceTable(config, this);
	return resTable;
}

GpuPipelineRef Dx12Device::InternalCreateGpuPipeline(const GpuPipelineConfig& config)
{
    return nullptr;
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
