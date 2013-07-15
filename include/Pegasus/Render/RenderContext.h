/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.h
//! \author David Worsham
//! \date   5th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#ifndef PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
#define PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H

#include "Pegasus/Window/WindowDefs.h"
#include <stdint.h>


namespace Pegasus {
namespace Render {

//! \class Configuration structure for a rendering context.
struct ContextConfig
{
public:
    // Basic ctor / dtor
    ContextConfig();
    ContextConfig(WindowHandle hwnd);
    ~ContextConfig();

    //! Opaque window handle
    WindowHandle mWindowHandle;

    //! Startup context flag, indicating how the context should be created
    bool mStartupContext;
};

//! \class Class that encapsulates an OGL rendering context.
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

    //! Context handle opaque type
    //! Can be converted to a pointer
    typedef uintptr_t RenderContextHandle;

    //! Opaque window handle
    WindowHandle mWindowHandle;

    //! Opaque context handle
    RenderContextHandle mRenderContextHandle;
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
