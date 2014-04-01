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

//! Shader GPU factory interface. Sets gpu data for a shader node (containing compilation flags and source)
class IShaderFactory
{
public:
    IShaderFactory(){}
    virtual ~IShaderFactory(){}

    //! callback to be called at application constructor. Sets the right allocators for the shader gpu data.
    //! \param allocator the allocator to set
    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator) = 0;
    
    //! Generates gpu data for a shader node. The node data passed must belong to the shader node being passed.
    //! function is called internally in GenerateData of ShaderStage
    //! \param shaderNode the shader node containing all the properties needed for compilation
    //! \param nodeData the node data containing and holding the corresponding gpu data
    virtual void GenerateShaderGpuData(ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData) = 0;

    //! Destroys and sets to nullptr the gpu data from a node data node
    //! \param nodeData the node data to remove the gpu data from.
    virtual void DestroyShaderGpuData (Pegasus::Graph::NodeData * nodeData) = 0;

    
    //! Generates gpu program data for a program node. The programNode must be the owner of the nodeData being passed
    //! \param programNode the program node to use
    //! \param nodeData the node data to have the gpu data injected into
    virtual void GenerateProgramGpuData(ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData) = 0;

    //! Destroys the allocated gpu data inside nodeData  
    //! \param nodeData the node data to remove the gpu data from.
    virtual void DestroyProgramGpuData (Pegasus::Graph::NodeData * nodeData) = 0;

};

}
}


#endif

