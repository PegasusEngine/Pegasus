/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowContext.h
//! \author David Worsham
//! \date   01st Nov 2013
//! \brief  Context interface for windows, for access to external parameters

#ifndef PEGASUS_WINDOW_IWINDOWCONTEXT_H
#define PEGASUS_WINDOW_IWINDOWCONTEXT_H

#include "Pegasus/Core/Io.h"

namespace Pegasus {
namespace Window {

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


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_IWINDOWCONTEXT_H
