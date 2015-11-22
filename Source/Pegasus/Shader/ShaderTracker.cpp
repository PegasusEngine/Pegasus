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
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Allocator/IAllocator.h"

using namespace Pegasus;
using namespace Pegasus::Utils;
using namespace Pegasus::Shader;

ShaderTracker::ShaderTracker(Alloc::IAllocator* alloc)
:
    mShaderSources(alloc), mProgramLinkages(alloc)
{
}

void ShaderTracker::InsertProgram(ProgramLinkage * program)
{
    mProgramLinkages.PushEmpty() = program;
}

void ShaderTracker::InsertShader (ShaderSource * shader)
{
    mShaderSources.PushEmpty() = shader;
}    

void ShaderTracker::DeleteProgram(ProgramLinkage * program)
{
    for (unsigned int i = 0; i < mProgramLinkages.GetSize(); ++i)
    {
        if (mProgramLinkages[i] == program)
        {
            mProgramLinkages.Delete(i);
            return;
        }
    }
}

void ShaderTracker::DeleteShader (ShaderSource * shader)
{
    for (unsigned int i = 0; i < mShaderSources.GetSize(); ++i)
    {
        if (mShaderSources[i] == shader)
        {
            mShaderSources.Delete(i);
            return;
        }
    }
}

ShaderSource* ShaderTracker::DeleteShader (int id)
{
    ShaderSource* stage = mShaderSources[id];
    mShaderSources.Delete(id);
    return stage;
}

ProgramLinkage* ShaderTracker::DeleteProgram(int id)
{
    ProgramLinkage* prog = mProgramLinkages[id];
    mProgramLinkages.Delete(id);
    return prog;
}

int ShaderTracker::FindShaderIndex(ShaderSource* shader) const
{
    for (unsigned int i = 0; i < mShaderSources.GetSize(); ++i)
    {
        if (mShaderSources[i] == shader)
        {
            return i;
        }
    }
    return -1;
}

int ShaderTracker::FindProgramIndex(ProgramLinkage* program) const
{
    for (unsigned int i = 0; i < mProgramLinkages.GetSize(); ++i)
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

ShaderSource*  ShaderTracker::GetShaderSource(int id) const
{
    if (id >= 0 && id < ShaderSize())
    {
        return mShaderSources[id];
    }
    return nullptr;
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
