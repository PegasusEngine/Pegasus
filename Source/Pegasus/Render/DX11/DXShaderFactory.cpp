/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXShaderFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 shader factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/ShaderFactory.h"
#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderStage.h"

class DXShaderFactory : public Pegasus::Shader::IShaderFactory
{
public:
    DXShaderFactory(){}
    virtual ~DXShaderFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator); 
    
    virtual void GenerateShaderGPUData(Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData);

    virtual void DestroyShaderGPUData (Pegasus::Graph::NodeData * nodeData);

    virtual void GenerateProgramGPUData(Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData);

    virtual void DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData);

};

void DXShaderFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
}

void DXShaderFactory::GenerateShaderGPUData(Pegasus::Shader::ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData)
{
}

void DXShaderFactory::DestroyShaderGPUData (Pegasus::Graph::NodeData * nodeData)
{
}

void DXShaderFactory::GenerateProgramGPUData(Pegasus::Shader::ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData)
{
}

void DXShaderFactory::DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData)
{
}



namespace Pegasus {
namespace Render
{

DXShaderFactory gShaderFactory;

Shader::IShaderFactory * GetRenderShaderFactory()
{
    return &gShaderFactory;
}

}
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
