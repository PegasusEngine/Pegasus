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
    virtual Io::IOManager* GetIOManager() = 0;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_IWINDOWCONTEXT_H
