/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowContext.h
//! \author David Worsham
//! \date   01st Nov 2013
//! \brief  Context interface for windows, for access to external parameters

#ifndef PEGASUS_WND_IWINDOWCONTEXT_H
#define PEGASUS_WND_IWINDOWCONTEXT_H

#include "Pegasus/Core/Io.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/AssetLib/AssetLib.h"

namespace Pegasus {
namespace Wnd {


//! Context interface for windows
class IWindowContext
{
public:
    //! Destructor
    virtual ~IWindowContext() {};


    //! Gets the IO manager for use by this window
    //! \return The IO manager.
    virtual Io::IOManager* GetIOManager() const = 0;

    //! Gets the node manager for use by this window
    //! \return The node manager.
    virtual Graph::NodeManager* GetNodeManager() const = 0;

    //! Gets the shader manager for use by this window
    //! \return The shader manager.
    virtual Shader::ShaderManager* GetShaderManager() const = 0;

    //! Gets the texture manager for use by this window
    //! \return The texture manager.
    virtual Texture::TextureManager* GetTextureManager() const = 0;

    //! Gets the texture manager for use by this window
    //! \return The texture manager.
    virtual Mesh::MeshManager* GetMeshManager() const = 0;

    //! Gets the timeline for use by this window
    //! \return The timeline.
    virtual Timeline::Timeline* GetTimeline() const = 0;

    //! Gets the blockscript manager to construct / create block script libraries / scripts
    //! \return the BlockScriptManager
    virtual BlockScript::BlockScriptManager* GetBlockScriptManager() const = 0;

    //! Gets the asset library
    //! \return AssetLibrary
    virtual AssetLib::AssetLib* GetAssetLib() const = 0;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_IWINDOWCONTEXT_H
