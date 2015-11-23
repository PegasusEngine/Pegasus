/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderStage.h
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Pegasus Shader Stage	
#ifndef PEGASUS_SHADER_STAGE_H
#define PEGASUS_SHADER_STAGE_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
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
class ProgramLinkage;
class ShaderTracker;

//! Shader Stage class, holds information about a shader stage
class ShaderStage : public ShaderSource 
{
    friend class ShaderManager;

public:
    //! Default constructor
    //! \param  nodeAllocator used for nodes
    //! \param  nodeDataAllocator used for data allocation of nodes
    ShaderStage(
        Alloc::IAllocator* nodeAllocator,
        Alloc::IAllocator* nodeDataAllocator
    );

    //! Destructor
    virtual ~ShaderStage();

    //! Returns the shader type from the extension
    //! \param extension - the extension to use
    //! \return the type to return.
    static ShaderType DeriveShaderTypeFromExt(const char* extension);

    //! Set the shader source
    //! \param  type the type of the source
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    void SetSource(ShaderType type, const char * src, int srcSize);

    //! Set the shader source
    //! \param  src the actual src string
    //! \param  buffSize precomputed string length
    virtual void SetSource(const char * src, int srcSize);

    //! Return the stage type
    //! \return the shader type
    ShaderType GetStageType() const { return mType; }

    //! Sets this shader type.
    //! \param the type to set
    void SetStageType(ShaderType type) { mType = type; }

    //! Sets the factory, which contains the render library implementation of shader
    //! compilation and linkage
    //! \param factory factory with GPU implementation to be set.
    void SetFactory(IShaderFactory * factory) { mFactory = factory; }

    //! Invalidates internal data, next update will cause a recompilation.
    void InvalidateData();

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    virtual void ReleaseDataAndPropagate();

    //! Factory construction function for node manager
    static Graph::NodeReturn CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Get the class instance name for this node object.
    virtual const char* GetClassInstanceName() const { return "ShaderStage"; }

    //! Registers a parent owner
    void RegisterParent(ProgramLinkage* programLinkage);

    //! Removes the parent passed
    void UnregisterParent(ProgramLinkage* programLinkage);

    //! Internally regenerates the data required for compilation. Propagates and links changes upwards if it has
    //! parents
    virtual void Compile();

    //! Clears the children includes including this shader
    void ClearChildrenIncludes();

    //! \param the shader source in
    void Include(ShaderSourceIn shaderSrc);

    //! derives a shader type from an asset
    //! \param the asset
    //! \return the shader type
    static ShaderType DeriveShaderType(const Pegasus::AssetLib::Asset* asset);

protected:
    //! callback which allocates shader data
    virtual Pegasus::Graph::NodeData * AllocateData() const;

    //! override that generates data. This will generate the shader GPU data using the factory
    virtual void GenerateData();

    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset);

private:
    PG_DISABLE_COPY(ShaderStage)
    Utils::Vector<ProgramLinkage*> mParentReferences; //! reference to a parent program
    Utils::Vector<ShaderSourceRef> mIncludeReferences; //! reference to a #include piece of source
    Alloc::IAllocator* mAllocator; //! Allocator to use when creating this object
    IShaderFactory   * mFactory; //! reference to GPU shader factory
    ShaderType         mType; //! type of shader stage
    bool               mIsInDestructor; //hack state variable to avoid destruction on a callback


};

typedef Pegasus::Core::Ref<ShaderStage> ShaderStageRef;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageIn;
typedef Pegasus::Core::Ref<ShaderStage> & ShaderStageInOut;
typedef Pegasus::Core::Ref<ShaderStage> ShaderStageReturn;

}
}

#endif
