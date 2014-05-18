/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IShaderFactory.h
//! \author Kleber Garcia
//! \date   23rd March 2014
//! \brief  Major factory interface which creates shader GPU data. Responsible for compilation

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

//! Shader GPU factory interface. Sets GPU data for a shader node (containing compilation flags and source)
class IShaderFactory
{
public:
    IShaderFactory(){}
    virtual ~IShaderFactory(){}

    //! callback to be called at application constructor. Sets the right allocators for the shader GPU data.
    //! \param allocator the allocator to set
    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator) = 0;
    
    //! Generates GPU data for a shader node. The node data passed must belong to the shader node being passed.
    //! function is called internally in GenerateData of ShaderStage
    //! \param shaderNode the shader node containing all the properties needed for compilation
    //! \param nodeData the node data containing and holding the corresponding GPU data
    virtual void GenerateShaderGPUData(ShaderStage * shaderNode, Pegasus::Graph::NodeData * nodeData) = 0;

    //! Destroys and sets to nullptr the GPU data from a node data node
    //! \param nodeData the node data to remove the GPU data from.
    virtual void DestroyShaderGPUData (Pegasus::Graph::NodeData * nodeData) = 0;

    
    //! Generates GPU program data for a program node. The programNode must be the owner of the nodeData being passed
    //! \param programNode the program node to use
    //! \param nodeData the node data to have the GPU data injected into
    virtual void GenerateProgramGPUData(ProgramLinkage * programNode, Pegasus::Graph::NodeData * nodeData) = 0;

    //! Destroys the allocated GPU data inside nodeData  
    //! \param nodeData the node data to remove the GPU data from.
    virtual void DestroyProgramGPUData (Pegasus::Graph::NodeData * nodeData) = 0;

};

}
}


#endif

