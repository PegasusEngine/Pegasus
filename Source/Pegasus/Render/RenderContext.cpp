/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.cpp
//! \author David Worsham
//! \date   5th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#include "Pegasus/Render/RenderContext.h"
#include "..\Source\Pegasus\Render\IRenderContextImpl.h"

namespace Pegasus {
namespace Render {

Context::Context(const ContextConfig& config)
{
    // Make the impl
    mPrivateImpl = IRenderContextImpl::CreateImpl(config);
}

//----------------------------------------------------------------------------------------

Context::~Context()
{
    // Unbind and destroy the context
    IRenderContextImpl::DestroyImpl(mPrivateImpl);
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
