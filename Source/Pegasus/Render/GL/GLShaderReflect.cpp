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

    void PopulateUniformTable(Alloc::IAllocator * allocator, GLuint programHandle, GLShaderReflect::UniformTable * table)
    {
        static const int UNIFORM_TABLE_INCREMENT = 16; //16 uniforms at a time
    
        int total = 0;
        glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &total);
        if (total > table->mTableSize)
        {
            // round up the table size by the table uniform increment size
            table->mTableSize = ((total / UNIFORM_TABLE_INCREMENT) + 1) * UNIFORM_TABLE_INCREMENT;

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
        for (int i = 0; i < total; ++i)
        {
            int nameLen = 0;
            GLShaderUniform * uniform = &table->mTable[i];
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
            uniform->mSlot = glGetUniformLocation(programHandle, uniform->mName);
            PG_ASSERT(uniform->mSlot != GL_INVALID_INDEX);
        }
    }

    void PopulateReflectionInfo(Alloc::IAllocator * allocator, GLuint programHandle, GLShaderReflect& information)
    {
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
            PopulateUniformTable(allocator, programHandle, &information.mUniformTable);
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
