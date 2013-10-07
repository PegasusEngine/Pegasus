/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.h
//! \author David Worsham
//! \date   5th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#ifndef PEGASUS_RENDER_RENDERCONTEXT_H
#define PEGASUS_RENDER_RENDERCONTEXT_H

#include "Pegasus/Render/RenderDefs.h"


namespace Pegasus {
namespace Render {
// Forward declarations
class IContextImpl;

//! Configuration structure for a rendering context.
struct ContextConfig
{
public:
    // Basic ctor / dtor
    ContextConfig();
    ContextConfig(DeviceContextHandle hdc);
    ~ContextConfig();

    //! Opaque context handle
    DeviceContextHandle mDeviceContextHandle;

    //! Startup context flag, indicating how the context should be created
    bool mStartupContext;
};

//! Class that encapsulates an OGL rendering context.
class Context
{
public:
    // Ctor / dtor
    Context(const ContextConfig& config);
    ~Context();

    // Binding API
    void Bind() const;
    void Unbind() const;

    // Present API
    void Swap() const;

private:
    // No copies allowed
    explicit Context(const Context& other);
    Context& operator=(const Context& other);


    //! Opaque context handle
    DeviceContextHandle mDeviceContextHandle;

    //! Opaque GL context handle
    RenderContextHandle mRenderContextHandle;
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
