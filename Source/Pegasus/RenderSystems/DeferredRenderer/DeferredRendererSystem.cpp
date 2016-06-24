/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DeferredRendererSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/DeferredRenderer/DeferredRendererSystem.h"
#include "Pegasus/BlockScript/BlockLib.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_DEFERREDRENDERER

using namespace Pegasus;
using namespace Pegasus::RenderSystems;

void DeferredRendererSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    //todo: place stuff here
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
