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

    class IResource : public Core::RefCounted
    {
    public:
        void* GetInternalData() const { return mInternalData; }
        void* GetInternalDataAux() const { return mInternalDataAux; }
        void SetInternalData(void* internalData) { mInternalData = internalData; }
        void SetInternalDataAux(void* internalDataAux) { mInternalDataAux = internalDataAux; }
    private:
        void* mInternalData;
        void* mInternalDataAux;
    };

    //!basic template type
    template<typename ConfigType>
    class BasicResource : public IResource
    {
    public:
        explicit BasicResource(Pegasus::Alloc::IAllocator* allocator) 
            : RefCounted(allocator), mInternalData(nullptr), mInternalDataAux(nullptr)
        {
        }

        //implemented by the internal API implementation
        virtual ~BasicResource();

        void SetConfig(const ConfigType& config) { mConfig = config; }
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

    typedef int InternalJobHandle;
    const InternalJobHandle InvalidJobHandle = -1;
    class InternalJobBuilder;

}

}
