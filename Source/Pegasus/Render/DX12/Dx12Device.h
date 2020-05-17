/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Device.h
//! \author Kleber Garcia
//! \date   May 25th 2018
//! \brief  Class that implements the dx12 device

#ifndef PEGASUS_DX12DEVICE_H
#define PEGASUS_DX12DEVICE_H

#include <D3D12.h>
#include <string>
#include "../Common/ADevice.h"
#include <Pegasus/Core/Ref.h>


struct IDXGIFactory4;
struct IDXGIAdapter4;

namespace Pegasus
{
namespace Alloc
{
    class IAllocator;
}

namespace Render
{

struct GraphicsCardInfos
{
    IDXGIFactory4* dxgiFactory = nullptr;
    struct Card {
		std::string description;
        IDXGIAdapter4* adapter4 = nullptr;
    }* infos = nullptr; 
	int cardCounts = 0;
    int usedIndex = 0;
};

class Dx12QueueManager;
class Dx12RDMgr;
class Dx12Texture;
class Dx12GpuProgram;
class Dx12Buffer;
struct BufferDesc;
struct TextureDesc;
struct Dx12ProgramDesc;
struct CanonicalCmdListResult;
class RootJob;

//! Windows specific device for open gl graphics api
class Dx12Device : public ADevice<Dx12Device>
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    Dx12Device(const DeviceConfig& config, Pegasus::Alloc::IAllocator* allocator);
    virtual ~Dx12Device();

    GraphicsCardInfos* GetGraphicsCardInfos() const { return sCardInfos; }
    inline ID3D12Device2* GetD3D() { return mDevice; }
    Dx12QueueManager* GetQueueManager() { return mQueueManager; }
    Dx12RDMgr* GetRDMgr() { return mRDMgr; }
    Io::IOManager* GetIOMgr() { return mIOManager; }
    Alloc::IAllocator* GetAllocator() { return mAllocator; }

	Core::Ref<Dx12GpuProgram> CreateGpuProgram();

    BufferRef InternalCreateBuffer(const BufferConfig& config);
    BufferRef InternalCreateUploadBuffer(size_t sz);
    TextureRef InternalCreateTexture(const TextureConfig& config);
    RenderTargetRef InternalCreateRenderTarget(const RenderTargetConfig& config);
    ResourceTableRef InternalCreateResourceTable(const ResourceTableConfig& config);
    GpuPipelineRef InternalCreateGpuPipeline();
    GpuRasterStateRef InternalCreateRasterState(const GpuRasterStateConfig& config);
	IDisplayRef InternalCreateDisplayConfig(const DisplayConfig& displayConfig);
    GpuSubmitResult InternalSubmit(const RootJob& rootJob, const CanonicalCmdListResult& result);
    void InternalWait(GpuWorkHandle workHandle);

private:

    static int sDeviceRefCounts;
    static GraphicsCardInfos* sCardInfos;
    Dx12QueueManager* mQueueManager;
    Dx12RDMgr* mRDMgr;

    ID3D12Device2* mDevice;
    Alloc::IAllocator* mAllocator;
    Io::IOManager* mIOManager;
#if PEGASUS_DEBUG
	ID3D12Debug* mDebugLayer;
#endif

};

}
}


#endif
