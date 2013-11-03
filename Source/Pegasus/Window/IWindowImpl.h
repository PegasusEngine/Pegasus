/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FileName.h
//! \author Firstname Lastname
//! \date   XX month year
//! \brief  brief

#ifndef PEGASUS_WINDOW_IWINDOWIMPL_H
#define PEGASUS_WINDOW_IWINDOWIMPL_H

#include "Pegasus/Window/Shared/WindowConfig.h"
#include "..\Source\Pegasus\Window\IWindowMessageHandler.h"

namespace Pegasus {
namespace Window {

//! Platform-specific interface for a window
class IWindowImpl
{
public:
    //! Destructor
    virtual ~IWindowImpl() {}


    //! Factory function to construct an impl
    //! Implement this function in your platform-specific impl.
    //! \param config Config struct for this impl.
    //! \param messageHandler Message handler object for this impl.
    //! \return Returned window impl.
    static IWindowImpl* CreateImpl(const WindowConfig& config, IWindowMessageHandler* messageHandler);

    //! Factory function to destroy an impl
    //! Implement this function in your platform-specific impl.
    //! \param impl Impl object to destroy.
    static void DestroyImpl(IWindowImpl* impl);


    //! Gets the handle for this window
    //! \return Window handle.
    virtual WindowHandle GetHandle() const = 0;


    //! Resize this window
    //! \param New width in pixels.
    //! \param New height in pixels.
    virtual void Resize(unsigned int width, unsigned int height) = 0;
};


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_IWINDOWIMPL_H
