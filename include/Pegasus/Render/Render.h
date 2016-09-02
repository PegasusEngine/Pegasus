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

#ifndef PARR_H
#define PARR_H

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

    //!basic template type
    template<typename ConfigType>
    class BasicResource : public Core::RefCounted
    {
    public:
        explicit BasicResource(Pegasus::Alloc::IAllocator* allocator) 
            : RefCounted(allocator), mInternalData(nullptr), mInternalDataAux(nullptr)
        {
        }
        void* GetInternalData() const { return mInternalData; }
        void* GetInternalDataAux() const { return mInternalDataAux; }
        void SetInternalData(void* internalData) { mInternalData = internalData; }
        void SetInternalDataAux(void* internalDataAux) { mInternalDataAux = internalDataAux; }
        const ConfigType& GetConfig() const { return mConfig; }
        void SetConfig(const ConfigType& config) { mConfig = config; }

        //implemented by the internal API implementation
        ~BasicResource();

    private:
        void* mInternalData;
        void* mInternalDataAux;
        ConfigType mConfig;
    };

    //! Enumeration representing the cube map face to set in a render target
    enum CubeFace
    {
        X, NX, Y, NY, Z, NZ
    };

    // primitive mode
    enum PrimitiveMode
    {        
        PRIMITIVE_TRIANGLE_LIST,
        PRIMITIVE_TRIANGLE_STRIP,
        PRIMITIVE_LINE_LIST,
        PRIMITIVE_LINE_STRIP,
        PRIMITIVE_POINTS,
        PRIMITIVE_AUTOMATIC,
        PRIMITIVE_COUNT
    };

    class Constants
    {
    public:
        static const int MAX_RENDER_TARGETS = 8;
    };

    //! Structure handle with the respective render target configuration
    struct RenderTargetConfig
    {
        int mWidth;
        int mHeight;
        Core::Format mFormat;
    public:
        RenderTargetConfig()
        : mWidth(-1), mHeight(-1), mFormat(Core::FORMAT_RGBA_8_UNORM)
        {
        }

        RenderTargetConfig(int width, int height)
        : mWidth(width), mHeight(height)
        {
        }
    };

    struct DepthStencilConfig
    {
    };

    //! Structure containing configuration for rasterizer state
    struct RasterizerConfig
    {
        enum PegasusDepthFunc
        {
            NONE_DF,
            GREATER_DF,
            LESSER_DF,
            GREATER_EQUAL_DF,
            LESSER_EQUAL_DF,
            EQUAL_DF,
            COUNT_DF
        };

        enum PegasusCullMode
        {
            NONE_CM,
            CCW_CM,
            CW_CM,
            COUNT_CM
        };

        RasterizerConfig ()
        :    mCullMode(NONE_CM),
             mDepthFunc(NONE_DF)
        { 
        }    

        PegasusCullMode  mCullMode;
        PegasusDepthFunc mDepthFunc;

       
    };


    //! Structure handle with the respective cube map configuration
    struct CubeMapConfig
    {
        int mWidth;
        int mHeight;
        Core::Format mFormat;
        CubeMapConfig() : mWidth(-1), mHeight(-1), mFormat(Core::FORMAT_RGBA_8_UNORM) {}
    };

    //! Structure handle wtih the respective 3d texture configurations
    struct VolumeTextureConfig
    {
        int mWidth;
        int mHeight;
        int mDepth;
        Core::Format mFormat;
        VolumeTextureConfig() : mWidth(-1), mHeight(-1), mDepth(-1), mFormat(Core::FORMAT_RGBA_8_UNORM) {}
    };

    //! Structure containing configuration for blending state
    struct BlendingConfig
    {
        enum BlendOperator
        {
            NONE_BO,
            ADD_BO,
            SUB_BO,
            COUNT_BO
        };

        enum Multiplicator
        {
            ZERO_M,
            ONE_M,
            COUNT_M
        };

        BlendingConfig()
        : mBlendingOperator(NONE_BO),
          mSource(ZERO_M),
          mDest(ZERO_M)
        {
        }
        
        BlendOperator mBlendingOperator;
        Multiplicator mSource;
        Multiplicator mDest;
    };

    //! Structure representing config of a buffer in memory
    struct BufferConfig
    {
        int mSize;
        BufferConfig() : mSize(0) {}
    };

    //! Container specifying rectangle viewport in pixel coordinates
    //! Represents a rectangle
    struct Viewport
    {
        int mXOffset;
        int mYOffset;
        int mWidth;
        int mHeight;
    public:
        Viewport()
        :
        mXOffset(-1), mYOffset(-1), mWidth(-1), mHeight(-1)
        {
        }

        Viewport(int x, int y, int width, int height)
        :
        mXOffset(x), mYOffset(y), mWidth(width), mHeight(height)
        {
        }

        Viewport(int width, int height)
        :
        mXOffset(0), mYOffset(0), mWidth(width), mHeight(height)
        {
        }
    };

    //! Structure representing a uniform location in a particular shader
    //! The name holds a copy of the actual uniform name.
    //! The internal values are obfuscated, not to be used.
    struct Uniform
    {
        char mName[PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN];
        int  mInternalIndex;
        int  mInternalOwner;
        int  mInternalVersion;
    public:
            Uniform()
            : mInternalIndex(-1), mInternalVersion(-1), mInternalOwner(-1)
            {
                mName[0] = 0;
            }
    };

    //! Sampler descriptor
    struct SamplerStateConfig
    {
        enum Filter
        {
            POINT,
            BILINEAR,
            ANISO
        };

        enum CoordMode
        {
            WARP,
            MIRROR,
            CLAMP,
            BORDER
        };

        Filter mFilter;
        CoordMode mUCoordMode;
        CoordMode mVCoordMode;
        CoordMode mWCoordMode;
        int mMipBias;
        int mMinMip;
        int mMaxMip;
        int mMaxAnisotropy;
        Math::ColorRGBA mBorderColor;

        SamplerStateConfig()
        : mFilter(BILINEAR),
          mUCoordMode(CLAMP),
          mVCoordMode(CLAMP),
          mWCoordMode(CLAMP),
          mMipBias(0),
          mMinMip(-1),
          mMaxMip(-1),
          mMaxAnisotropy(-1),
          mBorderColor(0.0f,0.0f,0.0f,0.0f)
        {
        }
        
    };

    //Resource definitions
    typedef BasicResource<BufferConfig>       Buffer;
    typedef BasicResource<RenderTargetConfig> RenderTarget;
    typedef BasicResource<CubeMapConfig>      CubeMap;
    typedef BasicResource<VolumeTextureConfig> VolumeTexture;
    typedef BasicResource<DepthStencilConfig> DepthStencil;
    typedef BasicResource<RasterizerConfig> RasterizerState;
    typedef BasicResource<BlendingConfig> BlendingState;
    typedef BasicResource<SamplerStateConfig> SamplerState;

    //Ref typedefs
    typedef Core::Ref<Buffer> BufferRef;
    typedef Core::Ref<RenderTarget> RenderTargetRef;
    typedef Core::Ref<CubeMap> CubeMapRef;
    typedef Core::Ref<VolumeTexture> VolumeTextureRef;
    typedef Core::Ref<DepthStencil> DepthStencilRef;
    typedef Core::Ref<RasterizerState> RasterizerStateRef;
    typedef Core::Ref<BlendingState> BlendingStateRef;
    typedef Core::Ref<SamplerState> SamplerStateRef;


    //! Dispatches a shader pipeline.
    //! \param program the shader program to dispatch
    void SetProgram (Shader::ProgramLinkageInOut program);

    //! Gets the internal gpu version of a program. Use to figure out if this program has changed for workflows
    //! \return every time the program gets compiled, its version increases
    int GetProgramVersion (Shader::ProgramLinkageInOut program);

    //! Dispatches a mesh.
    //! \param mesh that the system will dispatch.
    //! \WARNING: a shader must have been dispatched before this call.
    //!           not dispatching a shader will cause the mesh to be incorrectly rendered
    //!           or throw an assert
    void SetMesh (Mesh::MeshInOut mesh);

    //! Clears mesh streams and indices being set. Use only when using meshes as compute outputs.
    void UnbindMesh();

    //! Sets a viewport
    //! \param the viewport to set
    void SetViewport(const Viewport& viewport);

    //! Sets a viewport using the render target's dimensions
    //! \param the render target's whose dimensions be used as viewport
    void SetViewport(const RenderTargetRef& viewport);

    //! Sets a viewport using the depth stencil target's dimensions
    //! \param the depthstencil target, which dimensions used as viewport
    void SetViewport(const DepthStencilRef& viewport);

    //! Dispatches a render target
    //! \param render target to dispatch
    //! \NOTE clears the depth target being dispatched
    void SetRenderTarget (RenderTargetRef& renderTarget);

    //! Dispatches a render target 
    //! \param render target to dispatch
    //! \param depth stencil to be set
    void SetRenderTarget (RenderTargetRef& renderTarget, DepthStencilRef& depthStencil);

    //! Dispatches a set of render targets
    //! \param number of targets  to dispatch
    //! \param render target array to dispatch
    //! \note clears the depth target being dispatched
    //! \note renderTargetCount must be between 0 and MAX_RENDER_TARGETS
    void SetRenderTargets (int renderTargetCount, RenderTargetRef* renderTarget);

    //! Dispatches a set of render targets and depth stencil
    //! \param number of targets  to dispatch
    //! \param render target array
    //! \param depth stencil dispatched
    //! \note renderTargetCount must be between 0 and MAX_RENDER_TARGETS
    void SetRenderTargets (int renderTargetCount, RenderTargetRef* renderTarget, DepthStencilRef& depthStencil);

    //! Sets all color / depth and stencil to null 
    //! \note this is equivalent of calling SetRenderTargets(0, nullptr);
    void UnbindRenderTargets();

    //! Sets the default render target frame buffer (the basic window render target)
    //! Sets also the default depth stencil buffer
    void DispatchDefaultRenderTarget();

    //! Clears the selected buffers
    void Clear(bool color, bool depth, bool stencil);

    //! Sets the clear color
    //! \param col color vector, rgba to set the clear command to
    //! \note the components must be normalized (from 0 to 1)
    void SetClearColorValue(const Pegasus::Math::ColorRGBA& col);

    //! Sets the rasterizer state
    //! \param rasterState
    void SetRasterizerState(const RasterizerStateRef& rasterState);

    //! Sets the blending state
    //! \param blendingState
    void SetBlendingState(const BlendingStateRef blendignState);

    //! Sets the depth clear value
    //! \param the depth scalar value to clear to
    //! \note the value must be from 0 to 1
    void SetDepthClearValue(float d);

    //! Sets the primitive mode to render to.
    void SetPrimitiveMode(PrimitiveMode mode);

    //! Draws geometry.
    //! \note Requires: -Shader to be dispatched
    //!                 -Mesh to be dispatched
    void Draw();

    //! Fills a uniform reference by name
    //! \param program, the program containing the uniform to get
    //! \param outputUniform, the empty uniform structure to be filled containing the metadata required
    //! \param name, the constant name of the uniform that we will be trying to find in the uniform table
    //! \return boolean, true if the uniform was found, false otherwise
    bool GetUniformLocation(Shader::ProgramLinkageInOut program, const char * name, Uniform& outputUniform);

    //! Creates a buffer optimized for uniform usage
    //! \param bufferSize, the size of the buffer to be used in bytes
    //! \return outputBuffer the output struct to be filled
    //! \note Uniform buffers are dynamic and unable to get written through compute. They can only be bound as cbuffers.
    BufferRef CreateUniformBuffer(int bufferSize);
    
    //! Creates a buffer that will contain read / write access for compute.
    //! \param the size of the buffer
    //! \param elementCount - the number of elements this buffer contains. Must be non 0, so it can be treated as an Array when set as an UAV
    //! \param true if this buffer can be binded as a uniform. If false, it will only be able to get binded as a read buffer.
    //! \note for now, compute buffers are static, and inaccessible for the cpu.
    BufferRef CreateComputeBuffer(int bufferSize, int elementCount, bool makeUniformBuffer);

    //! Creates a sampler state to be set.
    //! \param config the config
    //! \return the sampler state to get returned.
    SamplerStateRef CreateSamplerState(const SamplerStateConfig& samplerConfig);

    //! Creates a render target with the assigned texture configuration
    //! \param configuration of the render target
    //! \return output render target to fill / create
    RenderTargetRef CreateRenderTarget(RenderTargetConfig& config);

    //! Creates a render target from a cube map
    //! \param CubeFace the face of the cube map to set this target to
    //! \param the cube map to use for this target
    //! \return the render target to be set
    RenderTargetRef CreateRenderTargetFromCubeMap(CubeFace targetFace, CubeMapRef& cubeMap); 

    //! Creates a cube map from a description
    //! \param config the cube map configuration
    //! \return the cube map
    CubeMapRef CreateCubeMap(const CubeMapConfig& config);

    //! Creates a volume texture from config
    //! \param config the volume texture config
    //! \return the created volume texture
    VolumeTextureRef CreateVolumeTexture(const VolumeTextureConfig& config);

    //! Creates a render target looking at a slice of the volume texture
    //! \param volume texture
    //! \param the slice in z to use as a render target
    //! \return the render target reference
    RenderTargetRef CreateRenderTargetFromVolumeTexture(int slice, VolumeTextureRef& volumeTexture);

    //! Creates a rasterizer state given a configuration.
    //! \param config the configuration structure
    //! \param output rasterizer state created
    RasterizerStateRef CreateRasterizerState(const RasterizerConfig& config);

    //! Creates a blending state given a configuration.
    //! \param config the configuration structure
    //! \param output blendign state created
    BlendingStateRef CreateBlendingState(const BlendingConfig& config);

    //! Gets the index buffer of a mesh data.
    //! \return the index buffer of this mesh data
    BufferRef GetIndexBuffer(Mesh::MeshDataRef meshData);

    //! Gets the vertex buffer of a mesh data.
    //! \param stream, the stream id to get. Must be < 8
    //! \return the vertex buffer of this mesh data    
    BufferRef GetVertexBuffer(Mesh::MeshDataRef meshData, int stream);

    //! Gets the draw indirect parameters of a mesh.
    //! Use this function to bind a compute shader and write into such data.
    BufferRef GetDrawIndirectBuffer(Mesh::MeshDataRef meshData);

    //! Memcpys a buffer to the gpu destination.
    //! \param dstBuffer the GPU destination buffer to copy to. This buffer must be a constant uniform buffer. Cannot
    //!                  do this operation to compute buffers. Compute buffers must be written through compute shaders only.
    //! \param src the source buffer to use
    //! \param size, the size of the src buffer to copy to dstBuffer in bytes. If -1, 
    //!        it will use the size registered in dstBuffer
    //! \param offset, the offset to use
    void SetBuffer(BufferRef& dstBuffer, const void * src, int size = -1, int offset = 0);

    //! Sets the uniform value to a texture
    //! \param u uniform to set the value to
    //! \param texture to set to the uniform slot
    //! \return boolean, true on success, false on error
    bool SetUniformTexture(Uniform& u, Texture::TextureInOut texture);

    //! Sets the uniform block buffer (constant buffer block).
    //! \param u uniform block to set the value to
    //! \param buffer to set to the uniform block slot
    //! \return boolean, true on success, false on error
    bool SetUniformBuffer(Uniform& u, const BufferRef& buffer);

    //! Sets a buffer as a resource 
    //! \param u uniform block to set the value to
    //! \param buffer to set as a resource.
    //! \return boolean, true on success, false on error
    bool SetUniformBufferResource(Uniform& u, const BufferRef& buffer);

    //! Sets the render target as a texture view in the specified uniform location 
    //! \param u uniform parameter to set the value
    //! \param renderTarget render target to set as a texture view
    //! \param index of the render target to set
    //! \return boolean, true on success, false on error
    bool SetUniformTextureRenderTarget(Uniform& u, const RenderTargetRef& renderTarget);

    //! Sets a cube map as a view
    //! \param u uniform parameter to set the value
    //! \param cubeMap cube map to set 
    //! \return boolean, true on success, false on error
    bool SetUniformCubeMap(Uniform& u, CubeMapRef& cubeMap);

    //! Sets a volume texture as a view
    //! \param u uniform parameter to set the value
    //! \param  volume texture to set
    //! \return boolean, true on success, false on error
    bool SetUniformVolume(Uniform& u, const VolumeTextureRef& volume);

    //! Sets the sampler state. Must have a program bound.
    //! \param sampler to set in such program
    //! \param the slot of the sampler.
    //! \note a program must be bound, otherwise this will fail.
    void SetComputeSampler(SamplerStateRef& sampler, int slot);

    //! Sets the sampler state. Must have a program bound.
    //! \param sampler to set in such program
    //! \param the slot of the sampler.
    //! \note a program must be bound, otherwise this will fail.
    void SetPixelSampler(SamplerStateRef& sampler, int slot);

    //! Sets the sampler state. Must have a program bound.
    //! \param sampler to set in such program
    //! \param the slot of the sampler.
    //! \note a program must be bound, otherwise this will fail.
    void SetVertexSampler(SamplerStateRef& sampler, int slot);

    //! function that internally cleans any dispatched programs / shaders / meshes
    //! from the global state.
    void CleanInternalState();

    //! Sets a compute output, in the given slot
    //! \param buffer to set as an output for compute
    //! \param slot the slot id as an output for compute
    void SetComputeOutput(BufferRef buffer, int slot);

    //! Sets a compute output, in the given slot
    //! \param volume texture to set as an output for compute
    //! \param slot the slot id as an output for compute
    void SetComputeOutput(VolumeTextureRef buffer, int slot);

    //! Unsets all the state of the buffer outputs.
    void UnbindComputeOutputs();

    //! Clears any resources binded for compute.
    void UnbindComputeResources();

    //! Clears any resources binded for vertex shaders.
    void UnbindPixelResources();

    //! Clears any resources binded for pixel shaders.
    void UnbindVertexResources();

    //! Dispatches a compute shader.
    //! \param x the number of threadgroups in x
    //! \param y the number of threadgroups in y
    //! \param z the number of threadgroups in z
    void Dispatch(unsigned int x, unsigned int y, unsigned int z);
}
}

#endif
