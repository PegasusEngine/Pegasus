/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file ShaderSource.cpp
//! \author Kleber Garcia
//! \date 3rd March 2015
//! \brief Pegasus Shader include source file

#include "Pegasus/Shader/ShaderTracker.h"
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Shader;

ShaderSource::ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
: Pegasus::Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
  Core::SourceCode(nodeDataAllocator),
  mAllocator(nodeAllocator)
#if PEGASUS_ENABLE_PROXIES
    , mShaderTracker(nullptr)
    , mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER
}

ShaderSource::~ShaderSource()
{
#if PEGASUS_ENABLE_PROXIES
    if (mShaderTracker != nullptr)
    {
        mShaderTracker->DeleteShader(this);
    }
    GRAPH_EVENT_DESTROY_USER_DATA(&mProxy, "ShaderSource", GetEventListener());
#endif
}

Pegasus::Graph::NodeReturn Pegasus::Shader::ShaderSource::CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ShaderSource", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ShaderSource(nodeAllocator, nodeDataAllocator);
}

//! editor metadata
#if PEGASUS_ENABLE_PROXIES
void ShaderSource::SetFullFilePath(const char * name)
{
    int len = 0;
    if (name)
    {
        mFullPath[0] = '\0';
        PG_ASSERT(Pegasus::Utils::Strlen(name) < METADATA_NAME_LENGTH * 2); //does it all fit?
        Pegasus::Utils::Strcat(mFullPath, name);
        int fullLen = Pegasus::Utils::Strlen(name);
        const char * nameString1 = Pegasus::Utils::Strrchr(name, '/');
        const char * nameString2 = Pegasus::Utils::Strrchr(name, '\\');
        const char * nameString = nameString1 > nameString2 ? nameString1 : nameString2;
        if (nameString != nullptr)
        {
            fullLen = fullLen - (nameString - name + 1);
            Pegasus::Utils::Memcpy(mName, nameString + 1, fullLen);
            mName[fullLen] = '\0';
            fullLen = nameString - name + 1;
            Pegasus::Utils::Memcpy(mPath, name, fullLen);
            mPath[fullLen] = '\0';
        }
        else
        {
            len = fullLen < ShaderSource::METADATA_NAME_LENGTH - 1 ? fullLen : ShaderSource::METADATA_NAME_LENGTH - 1;
            Pegasus::Utils::Memcpy(mName, name, len);
            mName[len] = '\0';
            mPath[0] = '\0';
        }
    }
} 
#endif

void ShaderSource::GenerateData()
{
}

Pegasus::Graph::NodeData * ShaderSource::AllocateData() const
{
    return nullptr;
}

void ShaderSource::InvalidateData()
{
    GeneratorNode::InvalidateData();
}
