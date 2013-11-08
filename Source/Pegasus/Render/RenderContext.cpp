/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.cpp
//! \author David Worsham
//! \date   05th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#include "Pegasus/Render/RenderContext.h"
#include "../Source/Pegasus/Render/IRenderContextImpl.h"

namespace Pegasus {
namespace Render {

Context::Context(const ContextConfig& config)
    : mAllocator(config.mAllocator)
{
    // Make the impl
    mPrivateImpl = IRenderContextImpl::CreateImpl(config, mAllocator);
}

//----------------------------------------------------------------------------------------

Context::~Context()
{
    // Unbind and destroy the context
    IRenderContextImpl::DestroyImpl(mPrivateImpl, mAllocator);
}

//----------------------------------------------------------------------------------------

void Context::Bind() const
{
    mPrivateImpl->Bind();
}

//----------------------------------------------------------------------------------------

void Context::Unbind() const
{
    mPrivateImpl->Unbind();
}

//----------------------------------------------------------------------------------------

void Context::Swap() const
{
    mPrivateImpl->Swap();
}


}   // namespace Render
}   // namespace Pegasus
