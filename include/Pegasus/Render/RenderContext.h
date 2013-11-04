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

#include "Pegasus\Render\RenderContextConfig.h"
#include "Pegasus\Render\GL\GLEWStaticInclude.h"

// Forward declarations
namespace Pegasus {
    namespace Render {
        class IRenderContextImpl;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Render {

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


    Memory::IAllocator* mAllocator; //!< Allocator for this object
    IRenderContextImpl* mPrivateImpl; //!< Private implementation -- platform-specific
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_PEGASUSRENDERCONTEXT_H
