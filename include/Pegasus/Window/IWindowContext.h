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
#include "Pegasus/Texture/TextureManager.h"

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

    //! Gets the texture manager for use by this window
    //! \return The texture manager.
    virtual Texture::TextureManager* GetTextureManager() const = 0;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_IWINDOWCONTEXT_H
