//*******  Deferred Render that applies analytical lights ************//
//                                                                    //
//********************************************************************//

#define THREADS_X 32
#define THREADS_Y 32

#include "RenderSystems/Lighting/GBuffer.h"

cbuffer Constants
{
    uint2 gTargetDimensions;
	float2 padding;
};


Texture2D<float4> GBuffer0Texture;
Texture2D<float4> GBuffer1Texture;
Texture2D<float> DepthTexture;

RWTexture2D<float4> OutputBuffer;

[numthreads(THREADS_X,THREADS_Y,1)]
void main(uint3 dti : SV_DispatchThreadId)
{
    uint2 coords = dti.xy;
    if (all(coords < gTargetDimensions))
    {
        GBuffer gbuffers;
        gbuffers.gbuffer0 = GBuffer0Texture[coords];
        gbuffers.gbuffer1 = GBuffer1Texture[coords];

        MaterialInfo matInfo = ReadFromGbuffers(gbuffers);

        //TODO: perform light tile sampling and perform computation:
        {
            float3 finalLighting = matInfo.color * dot(normalize(-float3(0.7,0.7,0.7)), matInfo.worldNormal) + matInfo.smoothness;
            OutputBuffer[coords] = float4(finalLighting, 1.0);
        }
    }
}
