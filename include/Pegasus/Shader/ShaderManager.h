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

class IShaderEventListener;
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
    //! Returns the shader tracker. Keeps track of all programs and shaders existant.
    //! \return tracker helper. Use it to iterate over shaders and programs
    const ShaderTracker * GetShaderTracker() const { return &mShaderTracker; }
#endif

#if PEGASUS_USE_GRAPH_EVENTS
    void RegisterEventListener(IShaderEventListener * eventListener) { mEventListener = eventListener; }
#endif

private:
    //! Internal function. registers the node types.
    void RegisterAllNodes();

    //! reference to the global node manager. Use it to allocate node data.
    Graph::NodeManager * mNodeManager;

    //! reference to the GPU factory. Passed to every node generated from this manager.
    IShaderFactory * mFactory;

#if PEGASUS_ENABLE_PROXIES
    //! shader tracker. To be used by proxy to track and iterate over shader nodes
    ShaderTracker mShaderTracker;
#endif

#if PEGASUS_USE_GRAPH_EVENTS
    IShaderEventListener * mEventListener;
#endif
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
