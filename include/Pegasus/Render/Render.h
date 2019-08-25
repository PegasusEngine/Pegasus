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

enum class ResourceType : int
{
    Texture,
    Buffer,
    ResourceTable,
    RenderTarget,
    GpuPipeline,
    Count
};

class ResourceLookupTable;

class IResource : public Core::RefCounted
{
public:
    unsigned GetOpaqueId() const { return m_opaqueId; }
    virtual ~IResource();

    ResourceType GetType() const { return mResourceType; }

protected:
    IResource(Alloc::IAllocator* allocator, ResourceType resType, ResourceLookupTable* resourceLookupTable);

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

protected:
    BasicResource(ResourceLookupTable* parentTable, Pegasus::Alloc::IAllocator* allocator)
        : IResource(allocator, GetResourceType<ConfigType>(), parentTable)
    {
    }

private:
    ConfigType mConfig;
};

class IDevice;

struct BufferConfig
{
};

struct TextureConfig
{
};

struct ResourceTableConfig
{
};

struct GpuPipelineConfig
{
};

struct RenderTargetConfig
{
};

template<> inline ResourceType GetResourceType<BufferConfig>() { return ResourceType::Buffer; }
template<> inline ResourceType GetResourceType<TextureConfig>() { return ResourceType::Texture; }
template<> inline ResourceType GetResourceType<ResourceTableConfig>() { return ResourceType::ResourceTable; }
template<> inline ResourceType GetResourceType<GpuPipelineConfig>() { return ResourceType::GpuPipeline; }
template<> inline ResourceType GetResourceType<RenderTargetConfig>() { return ResourceType::RenderTarget; }

typedef BasicResource<BufferConfig>  Buffer;
typedef BasicResource<TextureConfig>  Texture;
typedef BasicResource<ResourceTableConfig>  ResourceTable;
typedef BasicResource<RenderTargetConfig> RenderTarget;
typedef BasicResource<GpuPipelineConfig>  GpuPipeline;

typedef Core::Ref<IResource> IResourceRef;
typedef Core::Ref<Buffer> BufferRef;
typedef Core::Ref<Texture> TextureRef;
typedef Core::Ref<RenderTarget> RenderTargetRef;
typedef Core::Ref<ResourceTable> ResourceTableRef;
typedef Core::Ref<GpuPipeline> GpuPipelineRef;

//! Starts a new marker for gpu debugging.
//! \param marker - the marker string, null terminated.
inline void BeginMarker(const char* marker) {}

//! Ends a maker for gpu debugging.
inline void EndMarker() {}

}

}
