//*******  Deferred Render that applies analytical lights ************//
//                                                                    //
//********************************************************************//

#define THREADS_X 32
#define THREADS_Y 32

#include "RenderSystems/Camera/Common.h"
#include "RenderSystems/Lighting/LightingCore.h"

cbuffer Constants
{
    uint2 gTargetDimensions;
	uint2 gActiveLightCounts;
};

Texture2D<float4> GBuffer0Texture;
Texture2D<float4> GBuffer1Texture;
Texture2D<float> DepthTexture;
StructuredBuffer<LightInfo> LightInputBuffer;
RWTexture2D<float4> OutputBuffer;

[numthreads(THREADS_X,THREADS_Y,1)]
void main(uint3 dti : SV_DispatchThreadId)
{
    uint2 coords = dti.xy;

    if (all(coords < gTargetDimensions))
    {
		float2 clipSpaceCoords = ((float2)dti.xy) / ((float2)gTargetDimensions) * 0.5 + 0.5;
        GBuffer gbuffers;
        gbuffers.gbuffer0 = GBuffer0Texture[coords];
        gbuffers.gbuffer1 = GBuffer1Texture[coords];

        MaterialInfo matInfo = ReadFromGbuffers(gbuffers);

		float clipSpaceDepth = DepthTexture[coords];
		float4 homogeneousWorldPos = mul(float4(clipSpaceCoords,clipSpaceDepth,1.0),gInvViewProj);
		float3 worldPos = homogeneousWorldPos.xyz/homogeneousWorldPos.w;
        //TODO: perform light tile sampling and perform computation:
        {
			float3 diffuse = float3(0.0,0.0,0.0);
			float3 specular = float3(0.0,0.0,0.0);
			const uint MaxLights = 16;
			[loop]
			for (uint i = 0; i < gActiveLightCounts.x && i < MaxLights; ++i)
			{
				LightInfo lightInfo = LightInputBuffer[i];
				switch(GetLightInfoType(lightInfo))
				{
				case LIGHTTYPE_SPHERE:
					{
						SphereLight l;
						GetSphereLight(lightInfo,l);
						ApplySphereLight(worldPos,matInfo,l,diffuse,specular);
					}
					break;
				case LIGHTTYPE_SPOT:
				default:
					{
						SpotLight l;
						GetSpotLight(lightInfo,l);
						ApplySpotLight(worldPos,matInfo,l,diffuse,specular);
					}
					break;
				}
			} 
            OutputBuffer[coords] = float4(matInfo.color*diffuse + specular,1.0);
        }
    }
}
