#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/Shader/ProgramLinkage.h"


namespace Pegasus
{
namespace Graph
{
    class NodeManager;
}

namespace Shader
{

class IEventListener;


//! Shader stage configuration structure
struct ShaderStageProperties
{
    Pegasus::Shader::IUserData * mUserData; //! user data held by shader stage
    Pegasus::Shader::ShaderType mType; //! type for shader stage
    const char * mSource; //! the source string
    int          mSourceSize; //! the source string precomputed size
        
    ShaderStageProperties()
    :
    mUserData(nullptr),
    mType(Pegasus::Shader::SHADER_STAGE_INVALID),
    mSource(nullptr),
    mSourceSize(0)
    {
    }
};

//! Shader stage file loading configuration
struct ShaderStageFileProperties
{
    const char * mPath; //! path for file to load, null terminated string
    Pegasus::Io::IOManager * mLoader; //! file loader
    Pegasus::Shader::IEventListener * mEventListener; //!event listener, required for loading events
    Pegasus::Shader::IUserData      * mUserData; //!user data to be injected into shader stage
    
    ShaderStageFileProperties()
    :
    mPath(nullptr),
    mLoader(nullptr),
    mEventListener(nullptr),
    mUserData(nullptr)
    {
    }
};

class ShaderManager
{
public:
    explicit ShaderManager(Graph::NodeManager * nodeManager);
    virtual ~ShaderManager();

    //! Load a shader stage from a file.
    ProgramLinkageRef CreateProgram() const;

    //! Load a shader stage from a file.
    //! \param properties.mPath File containing shader stage. valid extensions are:
    //!             .ps - pixel shader
    //!             .vs - vertex shader
    //!             .tcs -tesselation control  shader
    //!             .tes -tesselation evaluation shader
    //!             .gs - geometry shader
    //!             .cs - compute shader
    //! \return null on error, stage on success 
    ShaderStageReturn LoadShaderStageFromFile(const Pegasus::Shader::ShaderStageFileProperties& properties);

    //! Creates a new stage structure
    //! \param properties constructor properties for shader stage
    ShaderStageReturn CreateShaderStage(const Pegasus::Shader::ShaderStageProperties& properties);   

private:
    void RegisterAllNodes();

    Graph::NodeManager * mNodeManager;
    
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
