#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/Proxy/ShaderManagerProxy.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Shader/ShaderTracker.h"
#endif

namespace Pegasus {
    namespace Core {
        namespace CompilerEvents {
            class ICompilerEventListener;
        }
    }

    namespace Graph {
        class NodeManager;
    }

    namespace AssetLib {
        class AssetLib;
    }
}

namespace Pegasus
{

namespace Shader
{

class IShaderFactory;

//! ShaderManager, manages and keeps up with shader data
class ShaderManager
{
public:
    ShaderManager(Graph::NodeManager * nodeManager, IShaderFactory * factory);
    virtual ~ShaderManager();

    //! Load a shader stage from a file.
    //! \param filename
    //!             .ps - pixel shader
    //!             .vs - vertex shader
    //!             .tcs -tesselation control  shader
    //!             .tes -tesselation evaluation shader
    //!             .gs - geometry shader
    //!             .cs - compute shader
    //! \return null on error, stage on success 
    ShaderStageReturn LoadShader(const char* filename);

    //! Load a shader stage from a file.
    //! \param filename
    //!             .h - loads a shader header
    //! \return null on error, stage on success 
    ShaderSourceReturn LoadHeader(const char* filename);

    //! Loads a program from a file asset
    ProgramLinkageRef LoadProgram(const char * filename);

    //! \return true if the asset passed is a program, false otherwise
    bool IsProgram(const AssetLib::Asset* asset) const;

    //! Requests creation of a Shader from an asset. This is the core function that populates a node from an asset
    //! \param asset the asset to be used as a database to create a node
    //! \return node pointer to return.
    ShaderStageReturn CreateShader(AssetLib::Asset* asset);

    //! Requests creation of a Shader from an asset. This is the core function that populates a node from an asset
    //! \param asset the asset to be used as a database to create a node
    //! \return node pointer to return.
    ShaderSourceReturn CreateHeader(AssetLib::Asset* asset);

    //! Requests creation of a Program from an asset. This is the core function that populates a node from an asset
    //! \param asset the asset to be used as a database to create a node
    //! \return node pointer to return.
    ProgramLinkageReturn CreateProgram(AssetLib::Asset* asset);

    //! Requests creation of a Program
    //! \param name of this program
    //! \return node pointer to return.
    ProgramLinkageReturn CreateProgram(const char* name);

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the shader manager
    //! \return Proxy associated with the shader manager
    //@{
    inline ShaderManagerProxy * GetProxy() { return &mProxy; }
    inline const ShaderManagerProxy * GetProxy() const { return &mProxy; }
    //@}

    //! Returns the shader tracker. Keeps track of all programs and shaders existant.
    //! \return tracker helper. Use it to iterate over shaders and programs
    const ShaderTracker * GetShaderTracker() const { return &mShaderTracker; }

#endif

#if PEGASUS_USE_GRAPH_EVENTS
    void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener) { mEventListener = eventListener; }
#endif

    //! Sets the asset factory of this manager.
    //! \param assetFactory the asset factory to set
    void SetAssetLib(AssetLib::AssetLib* assetLib)  { mAssetLib = assetLib; }

    //! Gets the asset factory of this manager.
    //! \return assetFactory the asset factory to get
    AssetLib::AssetLib* GetAssetLib() const  { return mAssetLib; }

private:
    //! Internal function. registers the node types.
    void RegisterAllNodes();

    //! Reference to the global node manager. Use it to allocate node data.
    Graph::NodeManager * mNodeManager;

    //! Reference to the GPU factory. Passed to every node generated from this manager.
    IShaderFactory * mFactory;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the shader manager
    ShaderManagerProxy mProxy;

    //! Shader tracker. To be used by proxy to track and iterate over shader nodes
    ShaderTracker mShaderTracker;

#endif

#if PEGASUS_USE_GRAPH_EVENTS
    Pegasus::Core::CompilerEvents::ICompilerEventListener * mEventListener;
#endif

    AssetLib::AssetLib* mAssetLib;
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
