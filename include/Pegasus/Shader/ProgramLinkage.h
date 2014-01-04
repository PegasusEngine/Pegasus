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
#include "Pegasus/Shader/RenderPlatProgramLinker.h"
#include "Pegasus/Shader/ProgramData.h"

namespace Pegasus
{
namespace Shader
{


// forward declarations
class IEventListener;
class IUserData;


//! Program linkage class. Represents a set of linked shader stages
class ProgramLinkage : public Pegasus::Graph::OperatorNode
{
public:

    //! Default constructor
    //! \param  nodeAllocator used for specific allocations of nodes
    //! \param  nodeDataAllocator used for specific data allocations
    ProgramLinkage(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Destructor
    virtual ~ProgramLinkage();

#if PEGASUS_SHADER_USE_EDIT_EVENTS
    //! Set event listener for shader stage
    //! \param eventListener the event listener interface
    void SetEventListener(IEventListener * eventListener);


    //! Set user data of particular shader stage
    //! \param userData user data returned on the execution of an event. Use this 
    void SetUserData(IUserData * userData);
#endif

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
    PG_DISABLE_COPY(ProgramLinkage);
    Pegasus::Shader::RenderPlatProgramLinker mInternalLinker;
    unsigned char mStageFlags;
};

typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageRef;
typedef const Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageIn;
typedef       Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageInOut;
typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageReturn;

}
}

#endif
