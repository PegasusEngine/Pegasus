/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext_Win32.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Platform-specific render context for Windows.

#ifndef PEGASUS_RENDER_RENDERCONTEXTWIN32_H
#define PEGASUS_RENDER_RENDERCONTEXTWIN32_H

#if PEGASUS_PLATFORM_WINDOWS
#include "..\Source\Pegasus\Render\IRenderContextImpl.h"
#include "Pegasus\Render\RenderContextConfig.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Pegasus {
namespace Render {


//! Platform-specific render context for Windows
class RenderContextImpl_Win32 : public IRenderContextImpl
{
public:
    //! Constructor
    RenderContextImpl_Win32(const ContextConfig& config);

    //! Destructor
    ~RenderContextImpl_Win32();


    // IRenderContextImpl interface
    virtual void Bind() const;
    virtual void Unbind() const;
    virtual void Swap() const;

private:
    // No copies allowed
    PG_DISABLE_COPY(RenderContextImpl_Win32);


    HDC mDeviceContextHandle; //!< Opaque context handle
    HGLRC mRenderContextHandle; //!< Opaque GL context handle
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
#endif  // PEGASUS_RENDER_RENDERCONTEXTWIN32_H
