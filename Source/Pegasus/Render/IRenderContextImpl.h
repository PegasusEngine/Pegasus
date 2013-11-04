/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IRenderContextImpl.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  PIMPL interface for platform-specific render contexts.

#ifndef PEGASUS_RENDER_IRENDERCONTEXTIMPL_H
#define PEGASUS_RENDER_IRENDERCONTEXTIMPL_H

#include "Pegasus/Render/RenderContextConfig.h"

namespace Pegasus {
namespace Render {

//! PIMPL interface for platform-specific render contexts
class IRenderContextImpl
{
public:
    //! Destructor
    virtual ~IRenderContextImpl() {}


    //! Factory function to construct an impl
    //! Implement this function in your platform-specific impl.
    //! \param config Config struct for this impl.
    //! \param Allocator used to create this impl.
    //! \return Returned context impl.
    static IRenderContextImpl* CreateImpl(const ContextConfig& config, Memory::IAllocator* alloc);

    //! Factory function to destroy an impl
    //! Implement this function in your platform-specific impl.
    //! \param impl Impl object to destroy.
    //! \param Allocator used to destroy this impl.
    static void DestroyImpl(IRenderContextImpl* impl, Memory::IAllocator* alloc);


    //! Binds this context to the current thread, making it active
    virtual void Bind() const = 0;

    //! Unbinds this context from the current thread, making it not current
    virtual void Unbind() const = 0;

    //! Swaps the backbuffer chain, presenting a completed image to the display
    virtual void Swap() const = 0;
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_IRENDERCONTEXTIMPL_H
