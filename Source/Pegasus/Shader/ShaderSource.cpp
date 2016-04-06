/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file ShaderSource.cpp
//! \author Kleber Garcia
//! \date 3rd March 2015
//! \brief Pegasus Shader include source file

#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Shader;

ShaderSource::ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
: Core::SourceCode(nodeAllocator, nodeDataAllocator),
  mAllocator(nodeAllocator)
#if PEGASUS_ENABLE_PROXIES
    , mProxy(this)
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER
}

ShaderSource::~ShaderSource()
{
#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_DESTROY_USER_DATA(&mProxy, "ShaderSource", GetEventListener());
#endif
}

Pegasus::Graph::NodeReturn Pegasus::Shader::ShaderSource::CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ShaderSource", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ShaderSource(nodeAllocator, nodeDataAllocator);
}

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
