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
}

namespace Pegasus
{
namespace Shader
{


// forward declarations
class IShaderFactory;
class IProgramProxy;
class ProgramProxy;
class ShaderManager;


//! Program linkage class. Represents a set of linked shader stages
class ProgramLinkage : public Pegasus::Graph::OperatorNode, public Pegasus::AssetLib::RuntimeAssetObject
{
    friend class ShaderManager;
#if PEGASUS_ENABLE_PROXIES
    //! give full access to proxy
    friend class ProgramProxy;
#endif
    
    GRAPH_EVENT_DECLARE_DISPATCHER(Pegasus::Core::CompilerEvents::ICompilerEventListener)

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
    virtual unsigned int GetMinNumInputNodes() const { return 0;}

    //! Maximum number of input nodes
    //! \return the maximum number of input nodes
    virtual unsigned int GetMaxNumInputNodes() const { return static_cast<unsigned int>(Pegasus::Shader::SHADER_STAGES_COUNT);}

    //! Static function that creates a program linkage
    //! nodeAllocator node allocator
    //! nodeDataAllocator node data allocator
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
    //! \param factory the shader GPU factory to be used internally for compilation process.
    void SetFactory (IShaderFactory * factory) { mFactory = factory; }

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    virtual void ReleaseDataAndPropagate();

    //! Invalidates internally the data holding the program, to force relinking
    virtual void InvalidateData();
        
    void SetManager(ShaderManager* manager) { mManager = manager; }

#if PEGASUS_ENABLE_PROXIES
    //! returns the name of this program.
    //! \return name of program
    const char * GetName() const { return mName; }
    
    //! sets the name of this shader to be used
    void SetName(const char * name);

    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return GetName(); }

    //! returns the proxy accessor for this program. Only available in dev mode.
    IProgramProxy * GetProxy() { return &mProxy; }

    //! Sets the shader tracker for shader / program book keeping
    void SetShaderTracker(ShaderTracker* shaderTracker) { mShaderTracker = shaderTracker; }
#endif

    //! \return true if this asset is a program, false otherwise
    static bool IsProgram(const Pegasus::AssetLib::Asset* asset);

protected:
    //! overrides, do not use
    virtual void AddInput(Pegasus::Graph::NodeIn node);
    
    //! Get the particular shader stage
    //! \param tyoe the type to get from this program
    //! \return the shader stage reference
    Pegasus::Shader::ShaderStageRef GetShaderStage(Pegasus::Shader::ShaderType type) const;

    //! shader compilation logic
    virtual void GenerateData();

    //! allocation of node data
    virtual Pegasus::Graph::NodeData* AllocateData() const;

    //! event listening of input removal 
    //! \param index callback when input is removed
    virtual void OnRemoveInput(unsigned int index);

    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset);

    virtual void OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset);

private:    
#if PEGASUS_ENABLE_PROXIES
    //! name meta data stuff
    static const int METADATA_NAME_LENGTH = 256;
    char mName[METADATA_NAME_LENGTH];
#endif

    PG_DISABLE_COPY(ProgramLinkage);    
    //! pointer to shader factory
    IShaderFactory * mFactory;
    
    //! bit mask describing the contents of this node.
    //! this allows this node to behave like a set with a static
    //! topology.
    unsigned char mStageFlags;

    //! Manager, required to load shaders correctly
    ShaderManager* mManager;

#if PEGASUS_ENABLE_PROXIES
    ProgramProxy mProxy;    
    ShaderTracker* mShaderTracker;
#endif

};

typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageRef;
typedef const Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageIn;
typedef       Pegasus::Core::Ref<ProgramLinkage> & ProgramLinkageInOut;
typedef       Pegasus::Core::Ref<ProgramLinkage>   ProgramLinkageReturn;

}
}

#endif
