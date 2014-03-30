/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ProgramLinkage.h
//! \author Kleber Garcia
//! \date   1st December 2013 
//! \brief  Represents the linkage of a shader set of stages using the pegasus node framwork	
#ifndef PEGASUS_SHADER_PROGRAM_LINKAGE_H
#define PEGASUS_SHADER_PROGRAM_LINKAGE_H

#include "Pegasus/Graph/OperatorNode.h"
#include "Pegasus/Shader/ShaderStage.h"

namespace Pegasus
{
namespace Shader
{


// forward declarations
class IEventListener;
class IUserData;
class IShaderFactory;


//! Program linkage class. Represents a set of linked shader stages
class ProgramLinkage : public Pegasus::Graph::OperatorNode
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    , public EventDispatcher
#endif
{
    friend class ShaderManager;
#if PEGASUS_ENABLE_PROXIES
    //! give full access to proxy
    friend class ProgramProxy;
#endif

public:

    //! Default constructor
    //! \param  nodeAllocator used for specific allocations of nodes
    //! \param  nodeDataAllocator used for specific data allocations
    ProgramLinkage(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Destructor
    virtual ~ProgramLinkage();

    //! Set program stage 
    //! \param shaderStage the shader stage to be added
    void SetShaderStage(Pegasus::Shader::ShaderStageIn shaderStage);   


    //! Minimum number of input nodes
    //! \return the minimum number of input nodes
    virtual unsigned int GetMinNumInputNodes() const { return 1;}

    //! Maximum number of input nodes
    //! \return the maximum number of input nodes
    virtual unsigned int GetMaxNumInputNodes() const { return static_cast<unsigned int>(Pegasus::Shader::SHADER_STAGES_COUNT);}

    //! Static function that creates a program linkage
    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Returns a shader stage node
    //! \param type the type to be queried
    //! \return the shader stage smart reference
    ShaderStageReturn FindShaderStage(Pegasus::Shader::ShaderType type) const;

    //! Returns a shader stage node by its input id.
    //! Get the max number of inputs with the GetNumInputs
    //! \param the id of the shader stage
    //! \return the shader stage smart reference
    ShaderStageReturn FindShaderStageInput(int i) const { return GetInput(i); }

    //! Removes and returns a shader stage node. To be called only by the shader manager
    //! \param type the type to be queried
    //! \return the shader stage smart reference, null if it did not find it
    ShaderStageReturn RemoveShaderStage(Pegasus::Shader::ShaderType type);


    //! Sets the factory, which contains the render library implementation of shader
    //! compilation and linkage
    void SetFactory (IShaderFactory * factory) { mFactory = factory; }

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    virtual void ReleaseDataAndPropagate();

#if PEGASUS_ENABLE_PROXIES
    const char * GetName() { return mName; }
    void SetName(const char * name);
#endif
protected:
    //! overrides, do not use
    virtual void AddInput(Pegasus::Graph::NodeIn node);
    
    //! Get the particular shader stage
    //! \return the shader stage reference
    Pegasus::Shader::ShaderStageRef GetShaderStage(Pegasus::Shader::ShaderType type) const;

    //! shader compilation logic
    virtual void GenerateData();

    //! allocation of node data
    virtual Pegasus::Graph::NodeData* AllocateData() const;

    //! event listening of input removal 
    virtual void OnRemoveInput(unsigned int index);

private:    
#if PEGASUS_ENABLE_PROXIES
    static const int METADATA_NAME_LENGTH = 256;
    char mName[METADATA_NAME_LENGTH];
#endif

    PG_DISABLE_COPY(ProgramLinkage);    
    IShaderFactory * mFactory;
    unsigned char mStageFlags;
};

typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageRef;
typedef const Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageIn;
typedef       Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageInOut;
typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageReturn;

}
}

#endif
