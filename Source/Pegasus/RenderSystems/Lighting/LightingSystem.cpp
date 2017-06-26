/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LightingSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/Lighting/LightingSystem.h"
#include "Pegasus/RenderSystems/Lighting/LightRig.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

using namespace Pegasus;
using namespace Pegasus::RenderSystems;
using namespace Pegasus::Lighting;

void LightRig_CreateLightRig(BlockScript::FunCallbackContext& context)
{
}

void LightRig_SetActiveLightRig(BlockScript::FunCallbackContext& context)
{
}

void LightRig_GetCulledLightBuffer(BlockScript::FunCallbackContext& context)
{
}

void RenderSystems::LightingSystem::Load(Core::IApplicationContext* appContext)
{
    mLightBuffer = Render::CreateStructuredReadBuffer(sizeof(LightRig::GpuLightBuffer), LightRig::MaxLightsCount);
}

void RenderSystems::LightingSystem::WindowUpdate(unsigned int width, unsigned int height)
{
}

void RenderSystems::LightingSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    Application::GenericResource::RegisterGenericResourceType(Pegasus::Lighting::LightRig::GetStaticClassInfo(), blocklib);

    BlockScript::FunctionDeclarationDesc extraFuns[3];

    extraFuns[0].functionName = "CreateLightRig";
    extraFuns[0].returnType = "LightRig";
    extraFuns[0].argumentTypes[0] = nullptr;
    extraFuns[0].argumentNames[0] = nullptr;
    extraFuns[0].callback = LightRig_CreateLightRig;

    extraFuns[1].functionName = "SetActiveLightRig";
    extraFuns[1].returnType = "int";
    extraFuns[1].argumentTypes[0] = "LightRig";
    extraFuns[1].argumentNames[0] = "lightRig";
    extraFuns[1].argumentTypes[1] = nullptr;
    extraFuns[1].argumentNames[1] = nullptr;
    extraFuns[1].callback = LightRig_SetActiveLightRig;

    extraFuns[2].functionName = "GetCulledLightBuffer";
    extraFuns[2].returnType = "Buffer";
    extraFuns[2].argumentTypes[0] = nullptr;
    extraFuns[2].argumentNames[0] = nullptr;
    extraFuns[2].callback = LightRig_GetCulledLightBuffer;

    blocklib->CreateIntrinsicFunctions(extraFuns, sizeof(extraFuns)/sizeof(extraFuns[0]));
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
