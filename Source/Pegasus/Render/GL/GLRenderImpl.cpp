/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLRenderImpl.cpp
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  OpenGL 4.X implementation of PARR

#if PEGASUS_GAPI_GL

#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"

//////////////////        GLOBALS CODE BLOCK     //////////////////////////////
//         All globals holding state data are declared on this blocka       ///
///////////////////////////////////////////////////////////////////////////////
//! helper state (in local thread storage) to keep track of current gpu state
//!
#if PEGASUS_PLATFORM_WINDOWS
    __declspec( thread )
#elif PEGASUS_PLATFORM_LINUX
    __thread
#else
    #error "Unknown platform, unable to use thread local storage keyword
#endif
struct GLState
{
    Pegasus::Render::OGLProgramGPUData  * mDispatchedShader;
    Pegasus::Render::OGLMeshGPUData * mDispatchedMeshGPUData;
} gOGLState  = { nullptr, nullptr };


// Must match the enum in MeshInputLayout for attribute types
GLenum gOGLAttrTypeTranslation[Pegasus::Mesh::MeshInputLayout::ATTRTYPE_COUNT] =
{
    GL_FLOAT,          //FLOAT
    GL_INT,            //INT
    GL_UNSIGNED_INT,   //UNSIGNED INT
    GL_SHORT,          //SHORT
    GL_UNSIGNED_SHORT, //USHORT
    GL_UNSIGNED_BYTE   //BOOL    
};

bool gEnableColorBuffer = false;

const GLuint MAX_TEX_BINDINGS = static_cast<int>(GL_TEXTURE31 - GL_TEXTURE0 + 1);

GLuint gActiveTex = GL_INVALID_INDEX;

struct GLTexBinding
{
    GLuint mHandle;
public:
    GLTexBinding()
    : mHandle(GL_INVALID_INDEX)
    {
    }
} gTexBindingCache[MAX_TEX_BINDINGS];

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetProgram FUNCTIONS IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetProgram (Pegasus::Shader::ProgramLinkageInOut program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * gpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::OGLProgramGPUData * programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(
         Pegasus::Render::OGLProgramGPUData, 
         gpuData
    );

    PG_ASSERT(programGPUData->mHandle != 0);
    glUseProgram(programGPUData->mHandle);
    gOGLState.mDispatchedShader = programGPUData;
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetMesh FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetMesh (Pegasus::Mesh::MeshInOut mesh)
{
    PG_ASSERTSTR(gOGLState.mDispatchedShader != nullptr, "Must dispatch a shader first in order to dispatch a mesh");
    Pegasus::Mesh::MeshDataRef meshData = mesh->GetUpdatedMeshData();
    Pegasus::Render::OGLMeshGPUData * meshGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLMeshGPUData, meshData->GetNodeGPUData());

    Pegasus::Render::OGLProgramGPUData * programGPUData = gOGLState.mDispatchedShader;


    if (programGPUData == nullptr)
    {
        PG_LOG('ERR_', "Error: must set first a program before setting a mesh");
        return;
    } 

    //find the correct VAO, otherwise, create a new entry
    // PEGASUS TODO: optimize this? if this proves to be a CPU bottleneck we will 
    //               have to weight this VAO table implementation with just doing VertexArrayPtr every frame
    Pegasus::Render::OGLMeshGPUData::VAOEntry * VAOEntry = nullptr;
    for (int i = 0; i < meshGPUData->mVAOTableCount; ++i)
    {
        if (programGPUData->mGUID == meshGPUData->mVAOTable[i].mProgramGUID)
        {
            VAOEntry = &meshGPUData->mVAOTable[i];
            break;
        }
    }

    if (VAOEntry == nullptr) //not found create a new VAOEntry
    {
        PG_ASSERTSTR(
            meshGPUData->mVAOTableCount < meshGPUData->mVAOTableSize,
            "VAO table is too small!, this is a fatal assert. We need to implement code into making this VAO"
            "table grow. If we never see this assert, then we will probably wont need to :)"
        );
        VAOEntry = &meshGPUData->mVAOTable[meshGPUData->mVAOTableCount++];
        VAOEntry->mProgramGUID = programGPUData->mGUID;
        
    }

    // bind the actual vertex array
    glBindVertexArray(VAOEntry->mVAOName);
    const Pegasus::Mesh::MeshConfiguration& meshConfig = meshData->GetConfiguration();
    
    //if the program has not changed, no need to re-record the vertex attribute array
    if (VAOEntry->mProgramVersion != programGPUData->mVersion)
    {
        VAOEntry->mProgramVersion = programGPUData->mVersion;
        //create the VAO commands
        int previousStream = -1;
        int currStreamStride = 0;
        const Pegasus::Mesh::MeshInputLayout * inputLayout = meshConfig.GetInputLayout();
        for (int i = 0; i < inputLayout->GetAttributeCount() ; ++i)
        {
            const Pegasus::Mesh::MeshInputLayout::AttrDesc& attrDesc = inputLayout->GetAttributeDesc(i);
            if (attrDesc.mStreamIndex != previousStream)
            {
                PG_ASSERT(meshGPUData->mBufferTable[attrDesc.mStreamIndex] != GL_INVALID_INDEX);
                glBindBuffer(GL_ARRAY_BUFFER, meshGPUData->mBufferTable[attrDesc.mStreamIndex]);
                currStreamStride =meshData->GetStreamStride(attrDesc.mStreamIndex);
                previousStream = attrDesc.mStreamIndex;
            }

            PG_ASSERTSTR(
                attrDesc.mSemantic >= 0 && attrDesc.mSemantic < Pegasus::Mesh::MeshInputLayout::SEMANTIC_COUNT &&
                attrDesc.mSemanticIndex >= 0 && attrDesc.mSemanticIndex < MESH_MAX_SEMANTIC_INDEX,
                "Semantic lookup out of bounds! this will get a garbage attribute location"
            );
            GLuint slot = programGPUData->mReflection.mAttributeLocations[attrDesc.mSemantic][attrDesc.mSemanticIndex];
            if (slot != GL_INVALID_INDEX)
            {
                PG_ASSERT(currStreamStride > 0);
                glVertexAttribPointer (
                    slot, 
                    attrDesc.mAttributeTypeCount, 
                    gOGLAttrTypeTranslation[attrDesc.mType], 
                    attrDesc.mIsNormalized ? GL_TRUE : GL_FALSE, 
                    currStreamStride, 
                    (void*)(attrDesc.mByteOffset)
                );
                glEnableVertexAttribArray(slot);
            }
            else
            {
                //TODO handle here case where an attribute is not used in a shader (i.e. depth pass not using normals)
            }
        }
    }
    
    if (meshGPUData->mDrawState.mIsIndexed)
    {
        PG_ASSERT(meshConfig.GetIsIndexed());
        PG_ASSERT(meshGPUData->mIndexBuffer != GL_INVALID_INDEX);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshGPUData->mIndexBuffer);
    }

    gOGLState.mDispatchedMeshGPUData = meshGPUData;
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetViewport FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetViewport(const Viewport& viewport)
{
    glViewport(viewport.mXOffset, viewport.mYOffset, viewport.mWidth, viewport.mHeight);  
}

void Pegasus::Render::SetViewport(const RenderTarget& viewport)
{
    glViewport(0, 0, viewport.mConfig.mWidth, viewport.mConfig.mHeight);  
}

void Pegasus::Render::SetViewport(const DepthStencilTarget& viewport)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SetRenderTargets FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRenderTarget (RenderTarget& renderTarget)
{
    RenderTarget* targetPtr = &renderTarget;
    Pegasus::Render::SetRenderTargets(1, &targetPtr);
}

void Pegasus::Render::SetRenderTarget (RenderTarget& renderTarget, DepthStencilTarget& depthStencil)
{
    RenderTarget* targetPtr = &renderTarget;
    Pegasus::Render::SetRenderTargets(1, &targetPtr, depthStencil);
}

void Pegasus::Render::SetRenderTargets (int renderTargetCount, RenderTarget** renderTargets)
{
    Pegasus::Render::SetRenderTargets(renderTargetCount, renderTargets, DepthStencilTarget());
}

void Pegasus::Render::SetRenderTargets (int renderTargetNum, RenderTarget** renderTarget, DepthStencilTarget& depthStencil)
{
    PG_ASSERT(renderTargetNum >= 0 && renderTargetNum < Pegasus::Render::Constants::MAX_RENDER_TARGETS);
    //TODO: handle depth
    if (renderTargetNum == 0)
    {
        ClearAllTargets();
        return;
    }

    Pegasus::Render::OGLRenderTargetGPUData * primaryGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLRenderTargetGPUData, renderTarget[0]->mInternalData);

    if (!gEnableColorBuffer)
    {
        glColorMask(true,true,true,true);
        gEnableColorBuffer = true;
    }

    //flush changes into the primary frame buffer name
    // this would be unlikely to occur
    glBindFramebuffer(GL_FRAMEBUFFER, primaryGpuData->mFrameBufferName);

    //if this render target is used for reading at any place, then unbind
    GLuint lastTexture = gActiveTex;

    for (int i = 1; i < GL_COLOR_ATTACHMENTS_COUNT; ++i)
    {
        if (i >= renderTargetNum)
        {
            if (primaryGpuData->mColAttachmentsCache[i] != 0)
            {
                primaryGpuData->mColAttachmentsCache[i] = 0;
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
            }
        }
        else
        {
            Pegasus::Render::OGLRenderTargetGPUData* secondaryTarget = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLRenderTargetGPUData, renderTarget[i]->mInternalData);
            if (primaryGpuData->mColAttachmentsCache[i] != secondaryTarget->mTextureView.mHandle)
            {
                primaryGpuData->mColAttachmentsCache[i] = secondaryTarget->mTextureView.mHandle;
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, primaryGpuData->mColAttachmentsCache[i], 0);
            }        

            //unbind this texture if its binded already
            for (GLuint b = 0; b < MAX_TEX_BINDINGS; ++b)
            {             
                GLTexBinding& binding = gTexBindingCache[b];
                GLuint activeTex = b + GL_TEXTURE0;
                if (secondaryTarget->mTextureView.mHandle == binding.mHandle)
                {
                    binding.mHandle = GL_INVALID_INDEX;
                    lastTexture = activeTex;
                    glActiveTexture(activeTex);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            } 
        }
    }

    //restore the original active texture slot
    if (lastTexture != gActiveTex)
    {
        glActiveTexture(gActiveTex);
    }
}

// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchDefaultRenderTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind the basic render target
}


///////////////////////////////////////////////////////////////////////////////
/////////////   ClearAllTargets FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::ClearAllTargets()
{
    glColorMask(false,false,false,false);
    glDepthMask(GL_FALSE);
    gEnableColorBuffer = false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind the basic render target
}

///////////////////////////////////////////////////////////////////////////////
/////////////   Clear Functions                         ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Clear(bool color, bool depth, bool stencil)
{
    GLuint flags = color ? GL_COLOR_BUFFER_BIT : 0;
    flags |= depth ? GL_DEPTH_BUFFER_BIT : 0;
    flags |= stencil ? GL_STENCIL_BUFFER_BIT : 0;
    glClear(flags);
}

void Pegasus::Render::SetClearColorValue(const Pegasus::Math::ColorRGBA& color)
{
    PG_ASSERTSTR(
        color.red >= 0.0 && color.red <= 1.0 &&
        color.green >= 0.0 && color.green <= 1.0 &&
        color.blue >= 0.0 && color.blue <= 1.0 &&
        color.alpha >= 0.0 && color.alpha <= 1.0,
        "Color ranges for clear must be normalized (from 0 to 1)"
    );
    glClearColor(
        color.red,
        color.green,
        color.blue,
        color.alpha
    );

}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETRASTERIZERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRasterizerState(const RasterizerState& rasterState)
{
    PG_ASSERT(rasterState.mInternalData == reinterpret_cast<void*>(0xdeadbeef));
    const RasterizerConfig& config = rasterState.mConfig;
    if (config.mCullMode == Pegasus::Render::RasterizerConfig::NONE_CM)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        static const GLenum sCullLookup[] = {
            0,
            GL_FRONT,
            GL_BACK
        };
        PG_ASSERT(config.mCullMode < (sizeof(sCullLookup) / sizeof(GLenum)));
        glCullFace(sCullLookup[config.mCullMode]);
    }   

    if (config.mDepthFunc == Pegasus::Render::RasterizerConfig::NONE_DF)
    {
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
        static const GLenum sDepthLookup[] = {
            0,
            GL_GREATER,
            GL_LESS,
            GL_GEQUAL,
            GL_LEQUAL,
            GL_EQUAL
        };
        PG_ASSERT(config.mDepthFunc < sizeof(sDepthLookup) / sizeof(GLenum));
        glDepthFunc(sDepthLookup[config.mDepthFunc]);
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBLENDINGSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetBlendingState(const Pegasus::Render::BlendingState& blendingState)
{
    PG_ASSERT(blendingState.mInternalData == reinterpret_cast<void*>(0xf00ba7));
    const Pegasus::Render::BlendingConfig& config = blendingState.mConfig;
    if (config.mBlendingOperator == Pegasus::Render::BlendingConfig::NONE_BO)
    {
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable(GL_BLEND);
        
        static const GLenum sBlendingFuncs[] = {
            0,
            GL_FUNC_ADD,
            GL_FUNC_SUBTRACT
        };
        PG_ASSERT(config.mBlendingOperator < sizeof(sBlendingFuncs) / sizeof(GLenum));
        glBlendEquation(sBlendingFuncs[config.mBlendingOperator]);

        static const GLenum sBlendingFuncMult[] = {
            GL_ZERO,
            GL_ONE
        };

        PG_ASSERT(config.mSource < sizeof(sBlendingFuncMult) / sizeof(GLenum));
        PG_ASSERT(config.mDest < sizeof(sBlendingFuncMult) / sizeof(GLenum));

        glBlendFunc(sBlendingFuncMult[config.mSource], sBlendingFuncMult[config.mDest]);
    }
}

// ---------------------------------------------------------------------------

void Pegasus::Render::SetDepthClearValue(float d)
{
    PG_ASSERTSTR(d >= 0.0 && d <= 1.0, "Depth clear value must be from 0 to 1");
    glClearDepth(d);
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DRAW FUNCTION IMPLEMENTATION      /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Draw()
{
    if (gOGLState.mDispatchedMeshGPUData == nullptr)
    {
        PG_LOG('ERR_', "A mesh must be set before calling draw!");
        return;
    }
    Pegasus::Render::OGLMeshGPUData::DrawState& drawState = gOGLState.mDispatchedMeshGPUData->mDrawState;
    
    if (drawState.mIsIndexed)
    {
        glDrawElements(drawState.mPrimitive, drawState.mIndexCount, GL_UNSIGNED_SHORT, (void*)0x0);
    }
    else
    {
        glDrawArrays(drawState.mPrimitive, 0, drawState.mVertexCount);
    }
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   GET UNIFORM FUNCTION IMPLEMENTATIONS    ///////////////////////
///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
//! Private function
//! Gets the location of a uniform by searching the uniform table of the specified program.
//! \param programGPUData the program GL and Reflection cached data.
//! \param outputUniform output uniform to fill with this data
//! \return true if the uniform was found and outputUniform was populated correctly, false otherwise
static bool GetUniformLocationInternal(Pegasus::Render::OGLProgramGPUData * programGPUData, Pegasus::Render::Uniform& outputUniform)
{
    Pegasus::Render::GLShaderReflect::UniformTable * table = &programGPUData->mReflection.mUniformTable;
    PG_ASSERT(table != nullptr);

    for (int i = 0; i < table->mTableCount; ++i)
    {
        Pegasus::Render::GLShaderUniform * uniformEntry = &table->mTable[i];
        if (!Pegasus::Utils::Strcmp(outputUniform.mName, uniformEntry->mName))
        {
            outputUniform.mInternalIndex = i;
            outputUniform.mInternalOwner = programGPUData->mGUID;
            outputUniform.mInternalVersion = programGPUData->mVersion;
            return true;
        }
    } 
    return false;
}

// ---------------------------------------------------------------------------
//! Private function
//! Attempts to recache the correct reflection uniform indices inside u. If the shader has not changed, this
//! functions becomes a nop
//! \param u the uniform to update if necesary. Otherwise (if it belogns to the current shader version then
//!        no touch
//! \return true if the uniform still exists, false otherwise
static bool UpdateUniform(Pegasus::Render::OGLProgramGPUData * dispatchedShader, Pegasus::Render::Uniform& u)
{
     
 
    if (dispatchedShader != nullptr && (u.mInternalOwner == dispatchedShader->mGUID || u.mInternalOwner == -1))
    {
        if (u.mInternalVersion != dispatchedShader->mVersion)
        {
            //uniform re-update is required
            return GetUniformLocationInternal(dispatchedShader, u);
        }
        else
        {
            return true;
        }
    }
    else
    {
        PG_LOG('ERR_', "Invalid shader dispatched.");
    }
    return false;
}

// ---------------------------------------------------------------------------
//! Private function
//! Checks the current uniform, updates it if necesary (if shader changed) otherwise keeps it
//! For example, if the shader has not changed, we simply return the GL internal data, if the shader
//! changed, we recache the uniform data in u and return the updated uniform. If for some reason the user
//! removed the uniform declaration in the shader, we return nullptr as the internal data.
//! This functions assumes a shader has been dispatched.
//! \param u the uniform to update if necesary, and to find its internal data.
//! \return returns the internal opengl handles and metadata, otherwise (if not found) returns nullptr.
static Pegasus::Render::GLShaderUniform * GetUpdatedGLUniformLocation(Pegasus::Render::Uniform& u)
{
    Pegasus::Render::OGLProgramGPUData * dispatchedShader = gOGLState.mDispatchedShader; 
    PG_ASSERTSTR(dispatchedShader, "A shader must be dispatched before setting a uniform");
    Pegasus::Render::GLShaderReflect::UniformTable * table = &dispatchedShader->mReflection.mUniformTable;
    PG_ASSERT(table != nullptr);
    bool result = UpdateUniform(dispatchedShader, u);
    if (result)
    {
        PG_ASSERT(u.mInternalIndex < table->mTableCount);   
        return &table->mTable[u.mInternalIndex];
    }
    else
    {
        return nullptr;
    }
}
// ---------------------------------------------------------------------------

bool Pegasus::Render::GetUniformLocation(Pegasus::Shader::ProgramLinkageInOut program, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * gpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    PG_ASSERT(gpuData != nullptr);

    Pegasus::Render::OGLProgramGPUData * programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(
         Pegasus::Render::OGLProgramGPUData, 
         gpuData
    );
    Pegasus::Utils::Memcpy(outputUniform.mName, name, PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN); 

    return GetUniformLocationInternal(programGPUData, outputUniform);
}


///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEUNIFORMBUFFER IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateUniformBuffer(int size, Pegasus::Render::Buffer& outputBuffer)
{
    PG_ASSERTSTR((size & 0x0f) == 0x0, "The size of the uniform buffer being allocated must be 16 byte aligned. Pack your buffer in chunks of 16 bytes (4 components of 4 bytes).");
    PG_ASSERTSTR(outputBuffer.mInternalData == 0, "Warning! buffer not deallocated, this will occur in a memory leak in the GPU");
    const GLuint usage = GL_DYNAMIC_DRAW; //set multiple times, used only in draw calls
    outputBuffer.mSize = size;
    GLuint name = GL_INVALID_INDEX;
    glGenBuffers(1, &name);
    PG_ASSERT(name != GL_INVALID_INDEX);
    outputBuffer.mInternalData = reinterpret_cast<void*>(name); //encode name in the lower dword

    //allocate memory
    glBindBuffer(GL_UNIFORM_BUFFER, name);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateRasterizerState(Pegasus::Render::RasterizerConfig& config, Pegasus::Render::RasterizerState& rasterizerState)
{
    rasterizerState.mConfig = config;
    rasterizerState.mInternalData = reinterpret_cast<void*>(0xdeadbeef);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateBlendingState(Pegasus::Render::BlendingConfig& config, Pegasus::Render::BlendingState& blendingState)
{
    blendingState.mConfig = config;
    blendingState.mInternalData = reinterpret_cast<void*>(0xf00ba7);
}
///////////////////////////////////////////////////////////////////////////////
/////////////   DELETERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRasterizerState(Pegasus::Render::RasterizerState& state)
{
    PG_ASSERT(state.mInternalData == reinterpret_cast<void*>(0xdeadbeef));
    //nop for OpenGL
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteBlendingState(Pegasus::Render::BlendingState& blendingState)
{
    PG_ASSERT(blendingState.mInternalData == reinterpret_cast<void*>(0xf00ba7));
    //nop for OpenGL
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBUFFER IMPLEMENTATION                ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetBuffer(Pegasus::Render::Buffer& dstBuffer, void * src, int size, int offset)
{
    GLuint sizeToUse = size == -1 ? dstBuffer.mSize : size;
    GLuint name = (GLuint) (dstBuffer.mInternalData);
    PG_ASSERTSTR(size + offset <= dstBuffer.mSize,"Warning! exceeding the byte capacity of this buffer. A memory stom will occur on the next calls.");
    glBindBuffer(GL_UNIFORM_BUFFER, name);
    glBufferSubData(GL_UNIFORM_BUFFER, (GLintptr)offset, sizeToUse, src);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::DeleteBuffer(Pegasus::Render::Buffer& buffer)
{
    PG_ASSERT(buffer.mInternalData != 0);
    glDeleteBuffers(1, (GLuint*)&buffer.mInternalData);
    //clear buffer
    buffer.mInternalData = 0;
    buffer.mSize = 0;
}

// ---------------------------------------------------------------------------

static bool InternalSetTextureUniform(Pegasus::Render::Uniform& u, Pegasus::Render::OGLTextureGPUData * gpuData)
{
    Pegasus::Render::GLShaderUniform * uniformEntry = GetUpdatedGLUniformLocation(u);
    if (uniformEntry != nullptr && uniformEntry->mSlot != GL_INVALID_INDEX)
    {
        PG_ASSERTSTR(
            uniformEntry->mType == GL_SAMPLER_1D ||
            uniformEntry->mType == GL_SAMPLER_2D ||
            uniformEntry->mType == GL_SAMPLER_3D ||
            uniformEntry->mType == GL_SAMPLER_CUBE ||
            uniformEntry->mType == GL_SAMPLER_1D_SHADOW ||
            uniformEntry->mType == GL_SAMPLER_2D_SHADOW,
        "Setting a uniform of the wrong type!");

        gActiveTex = GL_TEXTURE0 + uniformEntry->mTextureSlot;
        glActiveTexture(gActiveTex);
        
#if PEGASUS_ENABLE_ASSERT
        if (gpuData != nullptr)

#endif
        {
            glBindTexture(GL_TEXTURE_2D, gpuData->mHandle);
            glUniform1i(uniformEntry->mSlot, uniformEntry->mTextureSlot);
            PG_ASSERT(uniformEntry->mSlot >= 0 && uniformEntry->mSlot < MAX_TEX_BINDINGS);
            gTexBindingCache[uniformEntry->mSlot].mHandle = gpuData->mHandle;
            return true;
        }

    }
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformTexture(Pegasus::Render::Uniform& u, Pegasus::Texture::TextureInOut texture)
{
    Pegasus::Texture::TextureDataRef nodeData = texture->GetUpdatedTextureData();
    PG_ASSERT(nodeData->GetNodeGPUData() != nullptr);
    Pegasus::Render::OGLTextureGPUData * textureGPUData = 
        PEGASUS_GRAPH_GPUDATA_SAFECAST(
            Pegasus::Render::OGLTextureGPUData,
            nodeData->GetNodeGPUData()
        );
    return InternalSetTextureUniform(u, textureGPUData);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformBuffer(Pegasus::Render::Uniform& u, const Buffer& buffer)
{
    Pegasus::Render::GLShaderUniform * uniformEntry = GetUpdatedGLUniformLocation(u);
    if (uniformEntry != nullptr && uniformEntry->mSlot != GL_INVALID_INDEX)
    {
        if (uniformEntry->mUniformBlockSize != buffer.mSize)
        {
            PG_LOG('WARN',
            "Error setting uniform: %s. "
            "Please ensure its layout is std140 or that the size matches what the CPU is trying to push. "
            "Shader size: %d, Buffer size :%d", u.mName, uniformEntry->mUniformBlockSize, buffer.mSize);
        }        
        GLuint name = (GLuint) (buffer.mInternalData);
        glBindBuffer(GL_UNIFORM_BUFFER, name);
        glBindBufferBase(GL_UNIFORM_BUFFER, uniformEntry->mSlot, name);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformTextureRenderTarget(Pegasus::Render::Uniform& u, const RenderTarget& renderTarget)
{
    Pegasus::Render::GLShaderUniform * uniformEntry = GetUpdatedGLUniformLocation(u);

    PG_ASSERT(renderTarget.mInternalData != nullptr);

    Pegasus::Render::OGLRenderTargetGPUData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLRenderTargetGPUData, renderTarget.mInternalData);
    return InternalSetTextureUniform(u, &gpuData->mTextureView);
}

// ---------------------------------------------------------------------------
void Pegasus::Render::CleanInternalState()
{
    gOGLState.mDispatchedShader = nullptr;
    gOGLState.mDispatchedMeshGPUData = nullptr;
}
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif //PEGASUS_GAPI_GL
