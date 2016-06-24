/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

using namespace Pegasus;
using namespace Pegasus::RenderSystems;

void Terrain3dSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    //TODO: add blockscript functionality
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
