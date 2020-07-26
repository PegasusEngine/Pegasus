/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Render.h
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  Master render library. PARR - Pegasus Abstract Render Recipes	
//!         This library encapsulates most of the underlying API and serves as a way 
//!         of accessing it. Provides render shortcuts.

#pragma once

#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Core/Formats.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Core/Shared/EventDefs.h"

#include <string>
#include <vector>

namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }
}

#define PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN 64 

namespace Pegasus
{

namespace Render
{

class ResourceStateTable;
class IDevice;
class Buffer;
class Texture;
class GpuPipeline;

enum class ResourceType : int
{
    Texture,
    Buffer,
    ResourceTable,
    RenderTarget,
    Count
};

class IResource : public Core::RefCounted
{
public:
    friend IDevice;

    unsigned GetStateId() const { return mStateId; }
    virtual ~IResource();

    virtual void* GetGpuPtr() { return nullptr; }
    ResourceType GetType() const { return mResourceType; }

protected:
    IResource(IDevice* device, ResourceType resType);

private:
    unsigned mStateId;
    const ResourceType mResourceType;
    ResourceStateTable* mResourceStateTable;
    Alloc::IAllocator* mAllocator;
};

template<typename T>
ResourceType GetResourceType();

//!basic template type
template<typename ConfigType>
class BasicResource : public IResource
{
public:
    virtual ~BasicResource() {}
    void SetConfig(const ConfigType& config) { mConfig = config; }
    const ConfigType& GetConfig() const { return mConfig; }

protected:
    BasicResource(IDevice* device, const ConfigType& config)
        : IResource(device, GetResourceType<ConfigType>()), mConfig(config)
    {
    }

	ConfigType mConfig;
};


enum BindFlags : unsigned int
{
    BindFlags_Srv = 1,
    BindFlags_Cbv = 1 << 1,
    BindFlags_Uav = 1 << 2,
    BindFlags_Vb  = 1 << 3,
    BindFlags_Ib  = 1 << 4,
    BindFlags_Ds  = 1 << 5,
    BindFlags_Rt  = 1 << 6
};

enum BufferType : unsigned int
{
	BufferType_Default,
	BufferType_Structured,
	BufferType_Raw
};

enum ResourceUsage
{
    ResourceUsage_Static,
    ResourceUsage_Dynamic,
    ResourceUsage_Staging,
};

enum TextureType
{
    TextureType_1d,
    TextureType_2d,
    TextureType_3d,
    TextureType_CubeMap
};

enum ResourceTableType
{
    ResourceTableType_Srv,
    ResourceTableType_Uav
};

enum PipelineType : unsigned
{
    Pipeline_Vertex,
    Pipeline_Pixel,
    Pipeline_Domain,
    Pipeline_Hull,
    Pipeline_Geometry,
    Pipeline_Compute,
    Pipeline_Max,
    Pipeline_Unknown
};

struct ResourceConfig
{
    std::string name;
    Core::Format format;
    unsigned int bindFlags;
    ResourceUsage usage;
};

struct BufferConfig : public ResourceConfig
{
	unsigned int stride;
	unsigned int elementCount;
    BufferType bufferType;
};

struct TextureConfig : public ResourceConfig
{
    TextureType type;
    unsigned int width;
	unsigned int height;
    unsigned int depth;
    unsigned int mipLevels;
};

struct ResourceTableConfig
{
    ResourceTableType type = ResourceTableType_Srv;
    std::vector< Core::Ref<IResource> > resources;
};

struct RenderTargetConfig
{
	enum { MaxRt = 8 };
    unsigned colorCount = 1u;
	Core::Ref<Texture> colors[RenderTargetConfig::MaxRt];
	Core::Ref<Texture> depthStencil;
};

enum class DepthComparisonFunc
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class BlendVal
{
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor
};

enum class BlendOp
{
    Add, Subtract, Min, Max
};

enum class CullMode
{
    None, Ccw, Cw
};

struct GpuStateConfig
{
    struct Element
    {
        std::string semantic;
        Core::Format format = Core::FORMAT_RGBA_8_UNORM;
        bool perInstance = false;

        unsigned semanticIndex = 0u;
        unsigned streamId = 0u;
        unsigned instanceStepRate = 0u;
        int byteOffset = -1; //negative means we append
    };

    enum
    {
        MaxElements = 32
    };

    enum ColorBits
    {
        None   = 0,
        Red    = 1,
        Green  = 2,
        Blue   = 4,
        Alpha  = 8,
        All    = (ColorBits)( (int)Red | (int)Green | (int)Blue | (int)Alpha )
    };

    unsigned elementCounts = 1;
    Element elements[MaxElements];

    struct RenderTargetInfo
    {
        bool enableBlending = false;
        BlendVal blendSrc = BlendVal::SrcColor;
        BlendVal blendDest = BlendVal::Zero;
        BlendOp  blendOp = BlendOp::Add;
        BlendVal blendSrcAlpha = BlendVal::SrcAlpha;
        BlendVal blendDestAlpha = BlendVal::Zero;
        BlendOp  blendOpAlpha = BlendOp::Add;
        ColorBits colorBits = All;
        Core::Format format = Core::FORMAT_RGBA_8_UNORM;
    };

    struct DepthStencilDesc
    {
        bool depthEnable = true;
        Core::Format format = Core::FORMAT_R32_FLOAT;
        DepthComparisonFunc depthFunc = DepthComparisonFunc::LessEqual;
    };

    bool enableWireFrame = false;
    unsigned activeRenderTargets = 1;
    CullMode cullMode = CullMode::None;
    RenderTargetInfo renderTargetInfo[RenderTargetConfig::MaxRt];
    DepthStencilDesc dsDesc;
};

class GpuState : public Core::RefCounted
{
public:
    const GpuStateConfig& GetConfig() const { return mConfig; }

protected:
    GpuState(const GpuStateConfig& config, IDevice* device);
    GpuStateConfig mConfig;
};

struct GpuPipelineConfig
{
    const char* source = nullptr;
    unsigned sourceSize = 0u; 
    const char* mainNames[Pipeline_Max] = {};
    Core::Ref<GpuState> graphicsState;
};

template<> inline ResourceType GetResourceType<BufferConfig>() { return ResourceType::Buffer; }
template<> inline ResourceType GetResourceType<TextureConfig>() { return ResourceType::Texture; }
template<> inline ResourceType GetResourceType<ResourceTableConfig>() { return ResourceType::ResourceTable; }
template<> inline ResourceType GetResourceType<RenderTargetConfig>() { return ResourceType::RenderTarget; }

class Buffer : public BasicResource<BufferConfig> { public: Buffer(IDevice* device, const BufferConfig& config) : BasicResource<BufferConfig>(device, config) {} };
class Texture : public BasicResource<TextureConfig> { public: Texture(IDevice* device, const TextureConfig& config) : BasicResource<TextureConfig>(device, config) {} };
class ResourceTable : public BasicResource<ResourceTableConfig> { public: ResourceTable(IDevice* device, const ResourceTableConfig& config) : BasicResource<ResourceTableConfig>(device, config) {} };
class RenderTarget : public BasicResource<RenderTargetConfig> { public: RenderTarget(IDevice* device, const RenderTargetConfig& config) : BasicResource<RenderTargetConfig>(device, config) {} };

class GpuPipeline : public Core::RefCounted
{
public:
    PEGASUS_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

    virtual bool Compile(const GpuPipelineConfig& config) = 0;
    virtual bool IsValid() const = 0;

protected:
    GpuPipeline(IDevice* device);
    virtual ~GpuPipeline(){}
    void InvalidateData() {};
    IDevice* mDevice;
};

typedef Core::Ref<IResource> IResourceRef;
typedef Core::Ref<Buffer> BufferRef;
typedef Core::Ref<Texture> TextureRef;
typedef Core::Ref<ResourceTable> ResourceTableRef;
typedef Core::Ref<RenderTarget> RenderTargetRef;
typedef Core::Ref<GpuPipeline> GpuPipelineRef;
typedef Core::Ref<GpuState> GpuStateRef;

//! Starts a new marker for gpu debugging.
//! \param marker - the marker string, null terminated.
inline void BeginMarker(const char* marker) {}

//! Ends a maker for gpu debugging.
inline void EndMarker() {}

}

}
