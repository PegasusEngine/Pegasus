#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/Shader/ProgramLinkage.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Shader/ShaderTracker.h"
#endif

namespace Pegasus
{
namespace Graph
{
    class NodeManager;
}

namespace Shader
{

class IEventListener;
class IShaderFactory;



//! ShaderManager, manages and keeps up with shader data
class ShaderManager
{
public:
    ShaderManager(Graph::NodeManager * nodeManager, IShaderFactory * factory);
    virtual ~ShaderManager();

    //! Load a shader stage from a file.
    ProgramLinkageRef CreateProgram(const char * name = 0);

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

#if PEGASUS_ENABLE_PROXIES
    const ShaderTracker * GetShaderTracker() const { return &mShaderTracker; }
#endif

private:
    void RegisterAllNodes();

    Graph::NodeManager * mNodeManager;
    IShaderFactory * mFactory;

#if PEGASUS_ENABLE_PROXIES
    ShaderTracker mShaderTracker;
#endif
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
