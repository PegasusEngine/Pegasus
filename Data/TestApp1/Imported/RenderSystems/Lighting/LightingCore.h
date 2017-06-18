//*******  Lighting Core file             ************//
// Uber shader for lighting components                //
//****************************************************//

#ifndef USE_DEFERRED_RENDERER
#define USE_DEFERRED_RENDERER 1
#endif

#include "RenderSystems/Lighting/GBuffer.h"

#if USE_DEFERRED_RENDERER
#define LightingOutput GBuffer
#else
struct LightingOutput
{
    float4 out : SV_Target0;
};
#endif


LightingOutput ApplyLighting(in MaterialInfo matInfo)
{
#if USE_DEFERRED_RENDERER
    return WriteToGBuffers(matInfo);
#else
    LightingOutput output;
    output.out = ForwardLighting(matInfo);
    return output;
#endif
}

