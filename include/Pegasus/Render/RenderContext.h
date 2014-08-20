/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.h
//! \author David Worsham
//! \date   05th July 2013
//! \brief  Class that encapsulates a render library rendering context.

#ifndef PEGASUS_RENDER_RENDERCONTEXT_H
#define PEGASUS_RENDER_RENDERCONTEXT_H

#include "Pegasus/Render/RenderContextConfig.h"
#include "Pegasus/Core/Shared/OsDefs.h"

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Render {

typedef void* PrivateContextData;

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

    //! Triggered when the window has resized
    void Resize(int width, int height);

    //! Gets the parent device of this context
    IDevice * GetDevice() const { return mParentDevice; }
    
private:
    // No copies allowed
    PG_DISABLE_COPY(Context);

    Alloc::IAllocator* mAllocator; //!< Allocator for this object
    IDevice * mParentDevice;
    PrivateContextData mPrivateData; //!< Opaque context handle
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
