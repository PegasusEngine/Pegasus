/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXShaderFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 shader factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/ShaderFactory.h"


namespace Pegasus {
namespace Render
{

Shader::IShaderFactory * GetRenderShaderFactory()
{
    return nullptr;
}

}
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
