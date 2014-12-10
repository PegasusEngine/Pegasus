/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ScriptRenderApi.h
//! \author Kleber Garcia
//! \date   December 4th, 2014
//! \brief  BlockScript registration of Parr callbacks, so scripts can call, use and abuse
//!         of rendering nodes.

#ifndef SCRIPT_RENDER_API_H
#define SCRIPT_RENDER_API_H

#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/MeshGenerator.h"

//! forward declarations
namespace Pegasus {
    namespace Wnd {
        class IWindowContext;
    }
    namespace Alloc {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace Timeline
{
    //! registers the entire render API into blockscript's intrinsic library
    //! \param appContext - the context of the application
    void RegisterRenderApi(Pegasus::Wnd::IWindowContext* appContext);

    //! container of all the node types. Used by the script runtime.
    class NodeContainer{
    public:
        //naked accessors for elements of the node container
        Wnd::IWindowContext* mAppContext;
        Utils::Vector<Shader::ProgramLinkageRef> mPrograms;  
        Utils::Vector<Shader::ShaderStageRef>    mShaders;  
        Utils::Vector<Texture::TextureRef>       mTextures;  
        Utils::Vector<Texture::TextureGeneratorRef> mTexturesGenerators;  
        Utils::Vector<Texture::TextureOperatorRef>  mTexturesOperators;  
        Utils::Vector<Mesh::MeshRef>             mMeshes;  
        Utils::Vector<Mesh::MeshGeneratorRef>    mMeshesGenerators;  

        //! Constructor of node container
        NodeContainer(Alloc::IAllocator* alloc, Wnd::IWindowContext* context);
        
        //! Destructor of node container
        ~NodeContainer();

        //! Cleans / Deletes any existing node
        void Clean();
    };
}
}

#endif
