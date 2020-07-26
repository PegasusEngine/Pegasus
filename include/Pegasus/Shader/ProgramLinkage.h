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
#include "Pegasus/Shader/Proxy/ProgramProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"

namespace Pegasus {
    namespace Core {
        namespace CompilerEvents {
            class ICompilerEventListener;
        }
    }

    namespace AssetLib {
        class AssetLib;
        class Asset;
    }

    namespace Graph {
        class NodeManager;
    }

    namespace Render {
        class IDevice;
    }
}

namespace Pegasus
{
namespace Shader
{


// forward declarations
class IProgramProxy;
class ProgramProxy;
class ShaderManager;
class ShaderSource;

//! Program linkage class. Represents a set of linked shader stages
class ProgramLinkage : public Pegasus::Graph::OperatorNode, public Pegasus::AssetLib::RuntimeAssetObject
{
    friend class ShaderManager;
#if PEGASUS_ENABLE_PROXIES
    //! give full access to proxy
    friend class ProgramProxy;
#endif
    
    PEGASUS_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

public:

    //! Default constructor
    //! \param  nodeManager
    //! \param  nodeAllocator used for specific allocations of nodes
    //! \param  nodeDataAllocator used for specific data allocations
    ProgramLinkage(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Destructor
    virtual ~ProgramLinkage();

    //! Return the class instance name for this serializable object
    virtual const char* GetClassInstanceName() const { return "ProgramLinkage"; }

    //! Minimum number of input nodes
    //! \return the minimum number of input nodes
    virtual unsigned int GetMinNumInputNodes() const { return 0;}

    //! Maximum number of input nodes
    //! \return the maximum number of input nodes
    virtual unsigned int GetMaxNumInputNodes() const { return 1u;}

    //! Static function that creates a program linkage
    //! nodeAllocator node allocator
    //! nodeDataAllocator node data allocator
    static Graph::NodeReturn CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    void SetManager(ShaderManager* manager) { mManager = manager; }

    void SetSourceCode(Pegasus::Core::Ref<ShaderSource>& shaderSource);

    const Pegasus::Core::Ref<ShaderSource>& GetSourceCode() const { return mShaderSource; }

    void Compile();

#if PEGASUS_ENABLE_PROXIES
    //! returns the name of this program.
    //! \return name of program
    const char * GetName() const;
    
    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return GetName(); }

    //! returns the proxy accessor for this program. Only available in dev mode.
    virtual AssetLib::IRuntimeAssetObjectProxy * GetProxy() { return &mProxy; }
    virtual const AssetLib::IRuntimeAssetObjectProxy * GetProxy() const { return &mProxy; }
#endif

    virtual void InvalidateData() { Pegasus::Graph::OperatorNode::InvalidateData(); }

protected:
    //! overrides, do not use
    virtual void AddInput(Pegasus::Graph::NodeIn node);
    
    //! shader compilation logic
    virtual void GenerateData();

    //! allocation of node data
    virtual Pegasus::Graph::NodeData* AllocateData() const;

    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, const Pegasus::AssetLib::Asset* asset) override;

    virtual void OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset) override;

private:    
    PG_DISABLE_COPY(ProgramLinkage);    

    //! Manager, required to load shaders correctly
    ShaderManager* mManager;

    Pegasus::Core::Ref<ShaderSource> mShaderSource;

#if PEGASUS_ENABLE_PROXIES
    ProgramProxy mProxy;    
#endif

};

typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageRef;
typedef const Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageIn;
typedef       Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageInOut;
typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageReturn;

}
}

#endif
