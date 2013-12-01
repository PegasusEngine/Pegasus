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
#include "Pegasus/Shader/GL/GLProgramLinker.h"
#include "Pegasus/Shader/ProgramData.h"

namespace Pegasus
{
namespace Shader
{


// forward declarations
class IEventListener;
class IUserData;

//! Shader stage configuration structure
struct ShaderStageConfig
{
    Pegasus::Shader::IUserData * mUserData; // user data held by shader stage
    Pegasus::Shader::ShaderType mType; // type for shader stage
    const char * mSource; // the source string
    int          mSourceSize; // the source string precomputed size
        
    ShaderStageConfig()
    :
    mUserData(nullptr),
    mType(Pegasus::Shader::SHADER_STAGE_INVALID),
    mSource(nullptr),
    mSourceSize(0)
    {
    }
};

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

    //! Set event listener for shader stage
    //! \param eventListener the event listener interface
    void SetEventListener(IEventListener * eventListener);

    //! Set user data of particular shader stage
    //! \param userData user data returned on the execution of an event. Use this 
    void SetUserData(IUserData * userData);

    //! Set program stage configuration
    void SetShaderStage(const Pegasus::Shader::ShaderStageConfig& config);   

    //! Load a shader stage from a file.
    //! \param path File containing shader stage. valid extensions are:
    //!             .ps - pixel shader
    //!             .vs - vertex shader
    //!             .tcs -tesselation control  shader
    //!             .tes -tesselation evaluation shader
    //!             .gs - geometry shader
    //!             .cs - compute shader
    //! \param loader the pegasus loader control
    //! \return true on success, false on failre. See IShaderEvent to capture more detailed errors thrown
    bool LoadShaderStage(const char * path, Io::IOManager * loader);

    //! Minimum number of input nodes
    //! \return the minimum number of input nodes
    virtual unsigned int GetMinNumInputNodes() const { return 1;}

    //! Maximum number of input nodes
    //! \return the maximum number of input nodes
    virtual unsigned int GetMaxNumInputNodes() const { return static_cast<unsigned int>(Pegasus::Shader::SHADER_STAGES_COUNT);}

    //! Static function that creates a program linkage
    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

protected:
    //! overrides, do not use
    virtual void AddInput(Pegasus::Graph::NodeIn node);
    
    //! Get the particular shader stage
    //! \return the shader stage reference
    Pegasus::Shader::ShaderStageRef GetShaderStage(Pegasus::Shader::ShaderType type) const;

    //! shader compilation logic
    virtual void GenerateData();
    virtual Pegasus::Graph::NodeData* AllocateData() const;

private:
    PG_DISABLE_COPY(ProgramLinkage);
    Pegasus::Shader::GLProgramLinker mInternalLinker;
};

typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageRef;
typedef const Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageIn;
typedef       Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageInOut;
typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageReturn;

}
}

#endif
