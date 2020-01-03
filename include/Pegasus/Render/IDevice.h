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

struct PlatJobGraphHandle
{
    int id = -1;
    operator int() const { return id; }
    bool isValid() const { return id != -1; }
    PlatJobGraphHandle() = default;
	explicit PlatJobGraphHandle(PlatJobGraphHandle&& other) = default;
    explicit PlatJobGraphHandle(const PlatJobGraphHandle& other) = default;
	PlatJobGraphHandle& operator=(const PlatJobGraphHandle& other) = default;
    bool operator == (const PlatJobGraphHandle& other) const { return other.id == id; }
    bool operator != (const PlatJobGraphHandle& other) const { return !(*this == other); }
    bool operator <= (const PlatJobGraphHandle& other) const { return id <= other.id; }    
};

class IDevice
{
public:
    static IDevice * CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);

    virtual const DeviceConfig& GetConfig() const = 0;
    virtual Alloc::IAllocator * GetAllocator() const = 0;
    virtual BufferRef CreateBuffer(const BufferConfig& config) = 0;
    virtual TextureRef CreateTexture(const TextureConfig& config) = 0;
    virtual RenderTargetRef CreateRenderTarget(const RenderTargetConfig& config) = 0;
    virtual ResourceTableRef CreateResourceTable(const ResourceTableConfig& config) = 0;
    virtual GpuPipelineRef CreateGpuPipeline() = 0;
    virtual ResourceStateTable* GetResourceStateTable() const = 0;
};

}
}

#endif
