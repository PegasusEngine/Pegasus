/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/


//! \file   IDevice.h
//! \author Kleber Garcia
//! \date   7th July 2014
//! \brief  Class that encapsulates a drawing device

#pragma once

#include "Pegasus/Core/Shared/OsDefs.h"
#include <Pegasus/Render/Render.h>
#include <Pegasus/Render/IDisplay.h>

//forward declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }

    namespace Io
    {
        class IOManager;
    }
}

namespace Pegasus
{
namespace Render
{

class RootJob;
class ResourceStateTable;

enum class DevicePlat
{
 //   Vulcan, TODO: not available yet
    Dx12
};

//! Device configuration. Might vary per OS
struct DeviceConfig
{
    DevicePlat platform;
    Io::IOManager* mIOManager;
    Os::ModuleHandle mModuleHandle; //! handle to the HINSTANCE if windows, handle to the proc if linux
};

struct GpuWorkHandle
{
    int id = -1;
    operator int() const { return id; }
    bool isValid() const { return id != -1; }
    bool isValid(size_t maxVal) const { return isValid() && id < (int)maxVal; }
    GpuWorkHandle() = default;
	GpuWorkHandle(GpuWorkHandle&& other) = default;
    GpuWorkHandle(const GpuWorkHandle& other) = default;
	GpuWorkHandle& operator=(const GpuWorkHandle& other) = default;
	GpuWorkHandle& operator=(size_t val) { id = (int)val; return *this;	}
	bool operator == (const GpuWorkHandle& other) const { return other.id == id; }
    bool operator != (const GpuWorkHandle& other) const { return !(*this == other); }
    bool operator <= (const GpuWorkHandle& other) const { return id <= other.id; }    
};

enum class GpuWorkResultCode
{
    Success,
    InvalidArgs,
    CompileError,
    CompileError_Dependencies,
    CompileError_StaleJobs
};

struct GpuSubmitResult 
{
    GpuWorkHandle handle;
    GpuWorkResultCode resultCode;
};

class IDevice
{
public:
	virtual ~IDevice() {}
    static IDevice * CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);

    virtual const DeviceConfig& GetConfig() const = 0;
    virtual Alloc::IAllocator * GetAllocator() const = 0;
    virtual BufferRef CreateBuffer(const BufferConfig& config) = 0;

    virtual BufferRef CreateUploadBuffer(size_t sz) = 0;

    template<typename T>
    T* CreateUploadBuffer(BufferRef& outRef) {
        outRef =  CreateUploadBuffer(sizeof(T));
        return static_cast<T*>(outRef->GetGpuPtr());
    }

    virtual TextureRef CreateTexture(const TextureConfig& config) = 0;
    virtual RenderTargetRef CreateRenderTarget(const RenderTargetConfig& config) = 0;
    virtual ResourceTableRef CreateResourceTable(const ResourceTableConfig& config) = 0;
    virtual GpuPipelineRef CreateGpuPipeline() = 0;
    virtual IDisplayRef CreateDisplay(const DisplayConfig& displayConfig) = 0;


    virtual ResourceStateTable* GetResourceStateTable() const = 0;


    //Gpu Job Graph API

    //! SubmitWork
    //! Overview - Submits work to the gpu. Upon submission, the work can be tracked by using the GpuWorkHandle.
    //!            this can be used to access CPU resources on the user side.
    //!  rootJob - the root job of the work graph          
    //!  return - returns the result. The results contains an enum with the status. If ok, a handle can be used to track this work.
    virtual GpuSubmitResult Submit(const RootJob& rootJob) = 0;
    
    //! ReleaseWork 
    //! Overview - Work can be released anytime after rootJob has been submitted. Releasing work will simply free internal tracking resources
    //!            as soon as the GPU is finished, as well as CPU buffers that need to be filled by the GPU (that is they wont be alive).
    //!            Work can always be released immediately if desired, this wont stop the work happening on the GPU.
    //!            The application must always release the work to allow recycling of GPU->CPU internal heaps and memory.
    //!  GpuWorkHandle - a valid handle for work to be released.
    virtual void ReleaseWork(GpuWorkHandle workHandle) = 0;

    //! IsFinished
    //! Overview - Polling API to determine if work was finished in the GPU. Work can be released ANYTIME safely.
    //! return - True if the GPU work is finished. False otherwise.
    virtual bool IsFinished(GpuWorkHandle workHandle) = 0;

    //! Wait
    //! Overview - Blocking API. Will pause the current thread until the GPU has finished the work handle passed.
    virtual void Wait(GpuWorkHandle workHandle) = 0;

    //! Must be called every time a set of big workloads, or at least once per frame.
    //! This function will gather all 'garbage' and internal resources from released work.
    //! Garbage collect wont free CPU buffers of work handles that have not been released.
    virtual void GarbageCollect() = 0;
};

}
}
