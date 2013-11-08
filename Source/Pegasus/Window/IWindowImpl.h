/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowImpl.h
//! \author David Worsham
//! \date   02 November 2013
//! \brief  Platform-specific interface for a window.

#ifndef PEGASUS_WND_IWINDOWIMPL_H
#define PEGASUS_WND_IWINDOWIMPL_H

#include "Pegasus/Window/Shared/WindowConfig.h"
#include "../Source/Pegasus/Window/IWindowMessageHandler.h"

namespace Pegasus {
namespace Wnd {

//! Platform-specific interface for a window
class IWindowImpl
{
public:
    //! Destructor
    virtual ~IWindowImpl() {}


    //! Factory function to construct an impl
    //! Implement this function in your platform-specific impl.
    //! \param config Config struct for this impl.
    //! \param alloc Allocator used to create this impl.
    //! \param messageHandler Message handler object for this impl.
    //! \return Returned window impl.
    static IWindowImpl* CreateImpl(const WindowConfig& config, Alloc::IAllocator* alloc, IWindowMessageHandler* messageHandler);

    //! Factory function to destroy an impl
    //! Implement this function in your platform-specific impl.
    //! \param impl Impl object to destroy.
    //! \param alloc Allocator used to destroy this impl.
    static void DestroyImpl(IWindowImpl* impl, Alloc::IAllocator* alloc);


    //! Gets the handle for this window
    //! \return Window handle.
    virtual WindowHandle GetHandle() const = 0;


    //! Resize this window
    //! \param New width in pixels.
    //! \param New height in pixels.
    virtual void Resize(unsigned int width, unsigned int height) = 0;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_IWINDOWIMPL_H
