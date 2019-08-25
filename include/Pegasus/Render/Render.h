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

class ResourceLookupTable;
class IDevice;

enum class ResourceType : int
{
    Texture,
    Buffer,
    ResourceTable,
    RenderTarget,
    GpuPipeline,
    Count
};

class IResource : public Core::RefCounted
{
public:
    unsigned GetOpaqueId() const { return m_opaqueId; }
    virtual ~IResource();

    ResourceType GetType() const { return mResourceType; }

protected:
    IResource(IDevice* device, ResourceType resType);

private:
    unsigned m_opaqueId;
    const ResourceType mResourceType;
    ResourceLookupTable* mResourceLookupTable;
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
    BasicResource(IDevice* device)
        : IResource(device, GetResourceType<ConfigType>())
    {
    }

private:
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
    ResourceUsage_Staging
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

struct ResourceConfig
{
    std::string name;
    Core::Format format;
    unsigned int bindFlags;
    ResourceUsage usage;
};

struct TextureConfig : public ResourceConfig
{
    TextureType type;
    unsigned int width;
	unsigned int height;
    unsigned int depth;
    unsigned int mipLevels;
};

struct BufferConfig : public ResourceConfig
{
	unsigned int stride;
	unsigned int elementCount;
    BufferType bufferType;
};

struct ResourceTableConfig
{
    ResourceTableType type = ResourceTableType_Srv;
    std::vector< Core::Ref<IResource> > resources;
};

struct GpuPipelineConfig
{
};


template<> inline ResourceType GetResourceType<BufferConfig>() { return ResourceType::Buffer; }
template<> inline ResourceType GetResourceType<TextureConfig>() { return ResourceType::Texture; }
template<> inline ResourceType GetResourceType<ResourceTableConfig>() { return ResourceType::ResourceTable; }
template<> inline ResourceType GetResourceType<GpuPipelineConfig>() { return ResourceType::GpuPipeline; }

typedef BasicResource<BufferConfig>  Buffer;
typedef BasicResource<TextureConfig>  Texture;
typedef BasicResource<ResourceTableConfig>  ResourceTable;
typedef BasicResource<GpuPipelineConfig>  GpuPipeline;

typedef Core::Ref<IResource> IResourceRef;
typedef Core::Ref<Buffer> BufferRef;
typedef Core::Ref<Texture> TextureRef;
typedef Core::Ref<ResourceTable> ResourceTableRef;
typedef Core::Ref<GpuPipeline> GpuPipelineRef;


struct RenderTargetConfig
{
	enum { MaxRt = 8 };
	Core::Ref<Texture> colors[RenderTargetConfig::MaxRt];
	Core::Ref<Texture> depthStencil;
};

typedef BasicResource<RenderTargetConfig> RenderTarget;
template<> inline ResourceType GetResourceType<RenderTargetConfig>() { return ResourceType::RenderTarget; }
typedef Core::Ref<RenderTarget> RenderTargetRef;

//! Starts a new marker for gpu debugging.
//! \param marker - the marker string, null terminated.
inline void BeginMarker(const char* marker) {}

//! Ends a maker for gpu debugging.
inline void EndMarker() {}

}

}
