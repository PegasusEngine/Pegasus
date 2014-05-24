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

void Pegasus::Render::Dispatch (Pegasus::Shader::ProgramLinkageInOut program)
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

void Pegasus::Render::Dispatch (Pegasus::Mesh::MeshInOut mesh)
{
    PG_ASSERTSTR(gOGLState.mDispatchedShader != nullptr, "Must dispatch a shader first in order to dispatch a mesh");
    Pegasus::Mesh::MeshDataRef meshData = mesh->GetUpdatedMeshData();
    Pegasus::Render::OGLMeshGPUData * meshGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLMeshGPUData, meshData->GetNodeGPUData());

    Pegasus::Render::OGLProgramGPUData * programGPUData = gOGLState.mDispatchedShader;

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
    
    //if the program has not changed, no need to re-record the vertex attribute array
    if (VAOEntry->mProgramVersion != programGPUData->mVersion)
    {
        VAOEntry->mProgramVersion = programGPUData->mVersion;
        //create the VAO commands
        int previousStream = -1;
        int currStreamStride = 0;
        const Pegasus::Mesh::MeshConfiguration& meshCofig = meshData->GetConfiguration();
        const Pegasus::Mesh::MeshInputLayout * inputLayout = meshCofig.GetInputLayout();
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
    gOGLState.mDispatchedMeshGPUData = meshGPUData;
}


static bool GetUniformLocationInternal(Pegasus::Render::OGLProgramGPUData * programGPUData, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    Pegasus::Render::GLShaderReflect::UniformTable * table = &programGPUData->mReflection.mUniformTable;
    PG_ASSERT(table != nullptr);

    for (int i = 0; i < table->mTableCount; ++i)
    {
        Pegasus::Render::GLShaderUniform * uniformEntry = &table->mTable[i];
        if (!Pegasus::Utils::Strcmp(name, uniformEntry->mName))
        {
            Pegasus::Utils::Memcpy(outputUniform.mName, uniformEntry->mName, PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN); 
            outputUniform.mInternalIndex = i;
            outputUniform.mInternalOwner = programGPUData->mGUID;
            outputUniform.mInternalVersion = programGPUData->mVersion;
            return true;
        }
    } 
    return false;
}

bool Pegasus::Render::GetUniformLocation(Pegasus::Shader::ProgramLinkageInOut program, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * gpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    PG_ASSERT(gpuData != nullptr);

    Pegasus::Render::OGLProgramGPUData * programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(
         Pegasus::Render::OGLProgramGPUData, 
         gpuData
    );
    return GetUniformLocationInternal(programGPUData, name, outputUniform);
}

static bool UpdateUniform(Pegasus::Render::OGLProgramGPUData * dispatchedShader, Pegasus::Render::Uniform& u)
{
     
    PG_ASSERTSTR(u.mInternalOwner == dispatchedShader->mGUID, "The uniform does not belong to the shader being dispatched!");
    if (dispatchedShader != nullptr && u.mInternalOwner == dispatchedShader->mGUID)
    {
        if (u.mInternalVersion != dispatchedShader->mVersion)
        {
            //uniform re-update is required
            return GetUniformLocationInternal(dispatchedShader, u.mName, u);
        }
        else
        {
            return true;
        }
    }
    return false;
}

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

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, float value)
{
    Pegasus::Render::GLShaderUniform * uniformEntry = GetUpdatedGLUniformLocation(u);
    if (uniformEntry != nullptr)
    {
        PG_ASSERTSTR(uniformEntry->mType == GL_FLOAT, "Setting a uniform of the wrong type!");
        glUniform1f(uniformEntry->mSlot, value);
        return true;
    }
    return false;
}

void Pegasus::Render::Draw()
{
    PG_ASSERT(gOGLState.mDispatchedShader != nullptr);
    PG_ASSERT(gOGLState.mDispatchedMeshGPUData != nullptr);    
    Pegasus::Render::OGLMeshGPUData::DrawState& drawState = gOGLState.mDispatchedMeshGPUData->mDrawState;
    glDrawArrays(drawState.mPrimitive, 0, drawState.mVertexCount);
}

#endif
