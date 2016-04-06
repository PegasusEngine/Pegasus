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
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Graph/GeneratorNode.h"

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

    //! Return the stage type
    //! \return the shader type
    virtual ShaderType GetStageType() const { return SHADER_STAGE_INVALID; }

    //! Return the class instance name for this serializable object
    virtual const char* GetClassInstanceName() const { return "ShaderSource"; }

    static Graph::NodeReturn CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    virtual void InvalidateData();

#if PEGASUS_ENABLE_PROXIES
    virtual AssetLib::IRuntimeAssetObjectProxy * GetProxy() { return &mProxy; }
    virtual const AssetLib::IRuntimeAssetObjectProxy * GetProxy() const { return &mProxy; }
#endif


protected:
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
};

typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceRef;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceIn;
typedef Pegasus::Core::Ref<ShaderSource> & ShaderSourceInOut;
typedef Pegasus::Core::Ref<ShaderSource> ShaderSourceReturn;

}
}

#endif
