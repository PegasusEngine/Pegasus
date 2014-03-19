/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderTracker.cpp
//! \author Kleber Garcia
//! \date   3/17/2014 
//! \brief  Container class tracking all the shaders and programs defined in the app	
//!         Only available in dev mode
#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Shader/ShaderTracker.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderStage.h"

using namespace Pegasus::Shader;

static void AppendPointerList(void** list, int& size, void * element)
{
    if (size < ShaderTracker::MAX_SHADER_CONTAINER)
    {
        list[size] = element;
        ++size;
    }
    else
    {
        PG_FAILSTR("Increase container size for shader tracker");
    }
}

static void*  DeletePackPointerList(void** list, int& listSize, int id) 
{
    if (id >= 0 && id < listSize)
    {
        void * foundVal = list[id];
        //pack and delete backwards
        for (int i = id; i < listSize - 1; ++i)
        {
            list[i] = list[i + 1];
        }
        --listSize;
        
        return foundVal;
    }
    return nullptr;
}


static int FindPointerList( void * const * list , int listSize, void * element)
{
    for (int i = 0; i < listSize; ++i)
    {
        if (element == list[i]) return i;
    }
    return -1; 
}


ShaderTracker::ShaderTracker()
:
    mShaderSize(0), mProgramSize(0)
{
}

void ShaderTracker::InsertProgram(ProgramLinkage * program)
{
    AppendPointerList(mProgramLinkages, mProgramSize, program);    
}

void ShaderTracker::InsertShader (ShaderStage * shader)
{
    AppendPointerList(mShaderStages, mShaderSize, shader);    
}    

void ShaderTracker::DeleteProgram(ProgramLinkage * program)
{
    int id = FindProgramIndex(program);     
    DeleteProgram(id);
}

void ShaderTracker::DeleteShader (ShaderStage * shader)
{
    int id = FindShaderIndex(shader);     
    DeleteShader(id);
}

ShaderStage* ShaderTracker::DeleteShader (int id)
{
    return static_cast<ShaderStage*>(DeletePackPointerList(mShaderStages, mShaderSize, id)); 
}

ProgramLinkage* ShaderTracker::DeleteProgram(int id)
{
    return static_cast<ProgramLinkage*>(DeletePackPointerList(mProgramLinkages, mProgramSize, id)); 
}

int ShaderTracker::FindShaderIndex(ShaderStage* shader) const
{
    return FindPointerList(mShaderStages, mShaderSize, shader);
}

int ShaderTracker::FindProgramIndex(ProgramLinkage* program) const
{
    return FindPointerList(mProgramLinkages, mProgramSize, program);
}

ProgramLinkage*  ShaderTracker::GetProgram(int id) const
{
    if (id >= 0 && id < mProgramSize)
    {
        return static_cast<ProgramLinkage*>(mProgramLinkages[id]);
    }
    return nullptr;
}

ShaderStage*  ShaderTracker::GetShaderStage(int id) const
{
    if (id >= 0 && id < mShaderSize)
    {
        return static_cast<ShaderStage*>(mShaderStages[id]);
    }
    return nullptr;
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
