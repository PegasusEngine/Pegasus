/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file ShaderSource.h
//! \author Kleber Garcia
//! \date 3rd March 2015
//! \brief Pegasus Shader include source file

#ifndef PEGASUS_SHADER_SOURCE_H
#define PEGASUS_SHADER_SOURCE_H

#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Graph/GeneratorNode.h"
#include "Pegasus/Utils/Vector.h"

//fwd declarations
namespace Pegasus {

    namespace AssetLib {
        class IRuntimeAssetObjectProxy;
    }

    namespace Alloc {
        class IAllocator;
    }
    
    namespace Graph {
        class NodeManager;
    }

    namespace Shader {
        class ProgramLinkage;
    }
}

namespace Pegasus
{
namespace Shader
{

class ShaderSource : public Core::SourceCode 
{
public:
    //! constructor
    ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! destructor
    virtual ~ShaderSource();

    //! Return the class instance name for this serializable object
    virtual const char* GetClassInstanceName() const override { return "ShaderSource"; }

    static Graph::NodeReturn CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

#if PEGASUS_ENABLE_PROXIES
    virtual AssetLib::IRuntimeAssetObjectProxy * GetProxy() override { return &mProxy; }
    virtual const AssetLib::IRuntimeAssetObjectProxy * GetProxy() const override { return &mProxy; }
#endif

    void RegisterParent(ProgramLinkage* programLinkage);

    void UnregisterParent(ProgramLinkage* programLinkage);

    virtual void Compile() override;

protected:

    virtual void InvalidateData() override { mIsDirty = true; }

    Alloc::IAllocator* mAllocator;

    //! override that generates data. This will generate the shader GPU data using the factory
    virtual void GenerateData();

    //! callback which allocates shader data
    virtual Pegasus::Graph::NodeData * AllocateData() const;

private:

//! editor metadata
#if PEGASUS_ENABLE_PROXIES
    ShaderProxy mProxy;

#endif
    
    Utils::Vector<ProgramLinkage*> mProgramParents; //references to parents
    bool mIsDirty;
};

typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceRef;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceIn;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceInOut;
typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceReturn;

}
}

#endif
