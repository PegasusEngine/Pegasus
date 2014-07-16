/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderContext.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX implementation of render context

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/RenderContext.h"

namespace Pegasus {
namespace Render {

Context::Context(const ContextConfig& config)
    : mAllocator(config.mAllocator),
      mParentDevice(config.mDevice),
      mDeviceContextHandle(config.mDeviceContextHandle)
{
    PG_ASSERT(mParentDevice != nullptr);
}


Context::~Context()
{
}


void Context::Bind() const
{
}


void Context::Unbind() const
{ 
}


void Context::Swap() const
{
}

}//namespace Render
}//namespace Pegasus
#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
