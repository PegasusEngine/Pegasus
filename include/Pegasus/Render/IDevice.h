/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/


//! \file   IDevice.h
//! \author Kleber Garcia
//! \date   7th July 2014
//! \brief  Class that encapsulates a drawing device

#include "Pegasus/Core/Shared/OsDefs.h"
#include <Pegasus/Render/Render.h>

#ifndef PEGASUS_RENDER_DEVICE
#define PEGASUS_RENDER_DEVICE

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

class IDevice
{
public:
    //! Gets the config of this device
    const DeviceConfig& GetConfig() const { return mConfig; }

    virtual ~IDevice();
    
    //! Gets the allocator of this device
    Alloc::IAllocator * GetAllocator() const { return mAllocator; }

    //! Global function that creats the device specific to a platform
    static IDevice * CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);

    BufferRef CreateBuffer(const BufferConfig& config);
    TextureRef CreateTexture(const TextureConfig& config);
    RenderTargetRef CreateRenderTarget(const RenderTargetConfig& config);
    ResourceTableRef CreateResourceTable(const ResourceTableConfig& config);
    GpuPipelineRef CreateGpuPipeline();

    ResourceStateTable* GetResourceStateTable() const { return mResourceStateTable; }

protected:
    //! constructor, which creates the device
    //! \param config configuration stored
    //! \param allocator the allocator for internal use
    IDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);

    virtual BufferRef InternalCreateBuffer(const BufferConfig& config) = 0;
    virtual TextureRef InternalCreateTexture(const TextureConfig& config) = 0;
    virtual RenderTargetRef InternalCreateRenderTarget(const RenderTargetConfig& config) = 0;
    virtual ResourceTableRef InternalCreateResourceTable(const ResourceTableConfig& config) = 0;
    virtual GpuPipelineRef InternalCreateGpuPipeline() = 0;

private:
    Alloc::IAllocator * mAllocator;
    DeviceConfig mConfig;
    ResourceStateTable* mResourceStateTable;
};

}
}

#endif
