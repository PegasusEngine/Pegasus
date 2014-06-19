/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLShaderReflect.cpp

//! \author	Kleber Garcia
//! \date	12 May 2014
//! \brief	Shader reflection information. Cached at shader creation time.
//!         This information is used to determine the attribute locations for quick mesh dispatch

#if PEGASUS_GAPI_GL

#include "Pegasus/Allocator/IAllocator.h"
#include "../Source/Pegasus/Render/GL/GLShaderReflect.h"

namespace Pegasus
{
namespace Render
{

    void PopulateUniformTable(
        Alloc::IAllocator * allocator, 
        GLuint programHandle, 
        GLShaderReflect::UniformTable * table,
        bool& triggerAlignmentWarning
    )
    {
        static const int UNIFORM_TABLE_INCREMENT = 16; //16 uniforms at a time
    
        int totalUniforms = 0;
        glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &totalUniforms);

        int totalUniformBlocks = 0;
        glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_BLOCKS, &totalUniformBlocks);

        int total = totalUniforms + totalUniformBlocks;
    
        if (total > table->mTableSize)
        {
            // round up the table size by the table uniform increment size
            table->mTableSize = ((total/UNIFORM_TABLE_INCREMENT + 1)) * UNIFORM_TABLE_INCREMENT;

            // delete any previous data if existed, we need to grow the buffer
            if (table->mTable != nullptr)
            {
                PG_DELETE_ARRAY(allocator, table->mTable);
            }

            // allocate table memory
            table->mTable =  PG_NEW_ARRAY(
                allocator,
                -1,
                "Uniform Table",
                Pegasus::Alloc::PG_MEM_TEMP,
                GLShaderUniform,
                table->mTableSize
            );
        }

        // we know now the size of our table
        table->mTableCount = total;

        // table count must never exceed the table size
        PG_ASSERT(table->mTableCount <= table->mTableSize);

        // iterate over all the total uniforms that the shader has,
        // and cache their data into the uniform table
        int textureSlotAllocation = 0;
        for (int i = 0; i < totalUniforms; ++i)
        {
            int nameLen = 0;
            //re-initialize uniform by rerunning its constructor.
            GLShaderUniform * uniform = new(&table->mTable[i]) GLShaderUniform();

            int arraySize = 0;
            glGetActiveUniform(
                programHandle, 
                GLuint(i), 
                PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN - 1, 
                &nameLen,
                &arraySize,
                &uniform->mType,
                uniform->mName
            );
            PG_ASSERTSTR(arraySize == 1, "Uniform arrays not supported. Please use uniform blocks instead");
            uniform->mName[nameLen] = 0;
            uniform->mSlot = glGetUniformLocation(programHandle, uniform->mName); // uniform->mSlot == GL_INVALID_INDEX means is
                                                                                  // part of a uniform block
            if (
                   uniform->mType == GL_SAMPLER_1D ||
                   uniform->mType == GL_SAMPLER_2D ||
                   uniform->mType == GL_SAMPLER_3D ||
                   uniform->mType == GL_SAMPLER_CUBE ||
                   uniform->mType == GL_SAMPLER_1D_SHADOW ||
                   uniform->mType == GL_SAMPLER_2D_SHADOW
            )
            {
                uniform->mTextureSlot = textureSlotAllocation++;
            }
            else
            {
                uniform->mTextureSlot = -1; //invalid slot
            }            
        }

        //iterate over all the total uniform blocks the shader has
        for (int i = 0; i < totalUniformBlocks; ++i)
        {
            //re-initialize uniform by rerunning its constructor.
            GLShaderUniform * uniform = new(&table->mTable[i + totalUniforms]) GLShaderUniform();

            uniform->mType = GL_BUFFER;

            int nameLen = 0;

            glGetActiveUniformBlockName(
                programHandle,
                GLuint(i),
                PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN - 1,
                &nameLen,
                uniform->mName
            );

            uniform->mName[nameLen] = 0;
            
            glGetActiveUniformBlockiv (
                programHandle,
                GLuint(i),
                GL_UNIFORM_BLOCK_DATA_SIZE,
                &uniform->mUniformBlockSize
            );
                
            //if the uniform block is not 16 byte aligned, then do some error handling
            if  ((uniform->mUniformBlockSize & 0x0f) != 0x0)
            {
                PG_LOG(
                    'ERR_', 
                    "Your shader uniform block \"%s\" is not 16 byte (vec4) aligned. "
                    "Make sure to pack your uniform blocks in the shader in vec4 chunks (or add float padding). "
                    "Not doing so could cause compatibility issues with hardware vendors that misinterpret the std140 "
                    "alignment specification.",
                    uniform->mName
                );

                triggerAlignmentWarning = true;
            }

            uniform->mSlot = glGetUniformBlockIndex(programHandle, uniform->mName);
            PG_ASSERT(uniform->mSlot != GL_INVALID_INDEX);
        }
    }

    void PopulateReflectionInfo(
        Alloc::IAllocator * allocator, 
        GLuint programHandle, 
        GLShaderReflect& information,
        bool& triggerAlignmentWarning
    )
    {
        //assume no alignment issue
        triggerAlignmentWarning = false;

        PG_ASSERTSTR(MESH_MAX_SEMANTIC_INDEX < 10, " if semantic index is greater than 10, this algorithm needs to change!");
        //generate attribute name table
        for (int s = 0; s < Mesh::MeshInputLayout::SEMANTIC_COUNT; ++s)
        {
            for (int i = 0; i < MESH_MAX_SEMANTIC_INDEX; ++i)
            {
                information.mAttributeLocations[s][i] = GL_INVALID_INDEX;
                //if check worth every iteration since this happens at load time and is only 80 entries.
                //this also keeps the codespace small
                if (programHandle != 0) //if valid program handle
                {
                    char attrName[3];
                    attrName[0] = Mesh::MeshInputLayout::SemanticNames[s];
                    attrName[1] = '0' + i;
                    attrName[2] = 0;
                    information.mAttributeLocations[s][i] = glGetAttribLocation(programHandle, attrName);
                }
            }

        }

        information.mUniformTable.mTableCount = 0; // set the table count to zero, initialize all entries to invalid

        //generate shader uniform table
        if (programHandle != 0)
        {
            PopulateUniformTable(
                allocator, 
                programHandle, 
                &information.mUniformTable,
                triggerAlignmentWarning
            );
        }
    }
    
    void DestroyReflectionInfo(Pegasus::Alloc::IAllocator * alloc, GLShaderReflect& information)
    {
        // delete the dynamic uniform table memory
        if (information.mUniformTable.mTable != nullptr)
        {
            PG_DELETE_ARRAY(alloc, information.mUniformTable.mTable);
            information.mUniformTable.mTable = nullptr;
            information.mUniformTable.mTableCount = 0;
            information.mUniformTable.mTableSize  = 0;
        }
    }
}
}

#endif //Pegasus GL def
