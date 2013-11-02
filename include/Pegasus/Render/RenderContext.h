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

//! Configuration structure for a rendering context.
struct ContextConfig
{
public:
    //! Constructor
    ContextConfig();

    //! Destructor
    ~ContextConfig();


    DeviceContextHandle mDeviceContextHandle; //!< Opaque context handle
    bool mStartupContext; //!< Startup context flag, indicating how the context should be created
};

//----------------------------------------------------------------------------------------

//! Class that encapsulates an OGL rendering context.
class Context
{
public:
    //! Constructor
    //! \param config Config object for this context.
    Context(const ContextConfig& config);

    //! Destructor
    ~Context();


    //! Bind this context to the thread as the active context
    void Bind() const;

    //! Unbinds this context from the thread as the active context
    void Unbind() const;


    //! Swaps the backbuffer chain, presenting a completed image to the display
    void Swap() const;

private:
    // No copies allowed
    PG_DISABLE_COPY(Context);


    DeviceContextHandle mDeviceContextHandle; //!< Opaque context handle
    RenderContextHandle mRenderContextHandle; //!< Opaque GL context handle
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
