/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXMeshFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 mesh factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/MeshFactory.h"

namespace Pegasus
{

namespace Render
{

Mesh::IMeshFactory * GetRenderMeshFactory()
{
    return nullptr;
}

}

}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
