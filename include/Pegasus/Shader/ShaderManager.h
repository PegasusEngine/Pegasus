#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/AssetLib/AssetRuntimeFactory.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Shader/Proxy/ShaderManagerProxy.h"

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

//! ShaderManager, manages and keeps up with shader data
class ShaderManager : public AssetLib::AssetRuntimeFactory
{
public:
    //! Constructor
    //! \param nodeManager - the node manager
    //! \param the shader factory
    ShaderManager(Graph::NodeManager * nodeManager);

    //! Destructor
    virtual ~ShaderManager();

    //! Loads a program from a file asset
    //! \param program on success, otherwise null
    ProgramLinkageRef LoadProgram(const char * filename);

    //! Loads a program from a file asset
    //! \param program on success, otherwise null
    ShaderSourceRef LoadShaderSrc(const char * filename);

    //! Creates a new program.
    //! \return new program.
    ProgramLinkageReturn CreateProgram();

    //! Creates a new shader source.
    //! \return new source.
    ShaderSourceReturn CreateShaderSrc();

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the shader manager
    //! \return Proxy associated with the shader manager
    //@{
    inline ShaderManagerProxy * GetProxy() { return &mProxy; }
    inline const ShaderManagerProxy * GetProxy() const { return &mProxy; }
    //@}
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
#endif

#if PEGASUS_USE_EVENTS
    Pegasus::Core::CompilerEvents::ICompilerEventListener * mEventListener;
#endif
};

}//namespace Shader
}//namespace Pegasus

#endif //PEGASUS_SHADERMANAGER_H
