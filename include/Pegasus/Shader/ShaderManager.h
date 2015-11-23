#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/AssetLib/AssetRuntimeFactory.h"
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
class ShaderManager : public AssetLib::AssetRuntimeFactory
{
public:
    //! Constructor
    //! \param nodeManager - the node manager
    //! \param the shader factory
    ShaderManager(Graph::NodeManager * nodeManager, IShaderFactory * factory);

    //! Destructor
    virtual ~ShaderManager();

    //! Load a shader stage from a file.
    //! \return null on error, stage on success 
    ShaderStageReturn LoadShader(const char* filename);

    //! Load a shader header file, from a file.
    //! \param filename
    //! \return null on error, source on success 
    ShaderSourceReturn LoadHeader(const char* filename);

    //! Loads a program from a file asset
    //! \param program on success, otherwise null
    ProgramLinkageRef LoadProgram(const char * filename);

    //! Creates a new shader node.
    //! \return new shader node.
    ShaderStageReturn CreateShader();

    //! Creates a new shader header file.
    //! \return new header file.
    ShaderSourceReturn CreateHeader();

    //! Creates a new program.
    //! \return new program.
    ProgramLinkageReturn CreateProgram();

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

#if PEGASUS_USE_EVENTS
    void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener) { mEventListener = eventListener; }
#endif

protected:
    
    //! Creates a runtime object from an asset. This function must add a reference to the 
    //! runtime object returned, (if its ref counted)
    //! \param the asset type requested.
    //! \return the runtime asset created. return null if unsuccessfull.
    virtual AssetLib::RuntimeAssetObjectRef CreateRuntimeObject(const PegasusAssetTypeDesc* desc);

    //! Returns a null terminated list of asset descriptions this runtime factory will accept.
    //! \return a null terminated list of asset descriptions
    virtual const PegasusAssetTypeDesc*const* GetAssetTypes() const;

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

#if PEGASUS_USE_EVENTS
    Pegasus::Core::CompilerEvents::ICompilerEventListener * mEventListener;
#endif
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
