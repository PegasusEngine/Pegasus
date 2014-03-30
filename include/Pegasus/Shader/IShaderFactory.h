/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IShaderFactory.h
//! \author Kleber Garcia
//! \date   23rd March 2014
//! \brief  Major factory interface which creates shader gpu data. Responsible for compilation

#ifndef PEGASUS_ISHADERFACTORY_H
#define PEGASUS_ISHADERFACTORY_H

namespace Pegasus
{

namespace Alloc {
    class IAllocator;
}

namespace Graph {
    class NodeData;
}

namespace Shader
{

class ShaderStage;
class ProgramLinkage;

class IShaderFactory
{
public:
    IShaderFactory(){}
    virtual ~IShaderFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator) = 0;
    
    virtual void GenerateShaderGpuData(ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData) = 0;
    virtual void DestroyShaderGpuData (Pegasus::Graph::NodeData * nodeData) = 0;
    
    virtual void GenerateProgramGpuData(ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData) = 0;
    virtual void DestroyProgramGpuData (Pegasus::Graph::NodeData * nodeData) = 0;

};

}
}


#endif

