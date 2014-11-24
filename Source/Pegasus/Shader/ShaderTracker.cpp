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
#include "Pegasus/Allocator/IAllocator.h"

using namespace Pegasus;
using namespace Pegasus::Utils;
using namespace Pegasus::Shader;

ShaderTracker::ShaderTracker(Alloc::IAllocator* alloc)
:
    mShaderStages(alloc), mProgramLinkages(alloc)
{
}

void ShaderTracker::InsertProgram(ProgramLinkage * program)
{
    mProgramLinkages.PushEmpty() = program;
}

void ShaderTracker::InsertShader (ShaderStage * shader)
{
    mShaderStages.PushEmpty() = shader;
}    

void ShaderTracker::DeleteProgram(ProgramLinkage * program)
{
    for (int i = 0; i < mProgramLinkages.GetSize(); ++i)
    {
        if (mProgramLinkages[i] == program)
        {
            mProgramLinkages.Delete(i);
            return;
        }
    }
}

void ShaderTracker::DeleteShader (ShaderStage * shader)
{
    for (int i = 0; i < mShaderStages.GetSize(); ++i)
    {
        if (mShaderStages[i] == shader)
        {
            mShaderStages.Delete(i);
            return;
        }
    }
}

ShaderStage* ShaderTracker::DeleteShader (int id)
{
    ShaderStage* stage = mShaderStages[id];
    mShaderStages.Delete(id);
    return stage;
}

ProgramLinkage* ShaderTracker::DeleteProgram(int id)
{
    ProgramLinkage* prog = mProgramLinkages[id];
    mProgramLinkages.Delete(id);
    return prog;
}

int ShaderTracker::FindShaderIndex(ShaderStage* shader) const
{
    for (int i = 0; i < mShaderStages.GetSize(); ++i)
    {
        if (mShaderStages[i] == shader)
        {
            return i;
        }
    }
    return -1;
}

int ShaderTracker::FindProgramIndex(ProgramLinkage* program) const
{
    for (int i = 0; i < mProgramLinkages.GetSize(); ++i)
    {
        if (mProgramLinkages[i] == program)
        {
            return i;
        }
    }
    return -1;
}

ProgramLinkage*  ShaderTracker::GetProgram(int id) const
{
    if (id >= 0 && id < ProgramSize())
    {
        return mProgramLinkages[id];
    }
    return nullptr;
}

ShaderStage*  ShaderTracker::GetShaderStage(int id) const
{
    if (id >= 0 && id < ShaderSize())
    {
        return mShaderStages[id];
    }
    return nullptr;
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
