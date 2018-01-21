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
Texture2D<uint2> StencilTexture;
TextureCube<float4> SkyCube;

StructuredBuffer<LightInfo> LightInputBuffer;
RWTexture2D<float4> OutputBuffer : register(u0);

SamplerState bilinearSampler : register(s0);

[numthreads(THREADS_X,THREADS_Y,1)]
void main(uint3 dti : SV_DispatchThreadId)
{
    uint2 coords = dti.xy;

    if (all(coords < gTargetDimensions))
    {
		float4 hdrOutput =  float4(0.0,0.0,0.0,1.0);
		uint stencilVal = StencilTexture[coords].y;
		if (stencilVal)
		{
			float2 clipSpaceCoords = ((float2)dti.xy) / ((float2)gTargetDimensions)*2.0 - 1.0;
			clipSpaceCoords.y = -clipSpaceCoords.y;
			GBuffer gbuffers; 
			gbuffers.gbuffer0 = GBuffer0Texture[coords];
			gbuffers.gbuffer1 = GBuffer1Texture[coords];

			MaterialInfo matInfo = ReadFromGbuffers(gbuffers);		
			float clipSpaceDepth = DepthTexture[coords];
			float4 homogeneousWorldPos = mul(float4(clipSpaceCoords,clipSpaceDepth,1.0),gInvViewProj);
			float3 worldPos = homogeneousWorldPos.xyz/homogeneousWorldPos.w;
			float3 viewVector = normalize(gEyePos.xyz-worldPos);
			//TODO: perform light tile sampling and perform computation:
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
							ApplySphereLight(worldPos,matInfo,viewVector,l,diffuse,specular);
						}
						break;
					case LIGHTTYPE_SPOT:
					default:
						{
							SpotLight l;
							GetSpotLight(lightInfo,l);
							ApplySpotLight(worldPos,matInfo,viewVector,l,diffuse,specular);
						}
						break; 
					}
				}  
            /*WorldNormal debug*///float4(matInfo.worldNormal*0.5 + 0.5,1.0);
			/*WorldPos debug*///OutputBuffer[coords] = float4(worldPos.xyz*0.01, 1.0);
			/*Color debug*///OutputBuffer[coords] = float4(matInfo.color, 1.0);	
			specular += matInfo.reflectance*SkyCube.SampleLevel(bilinearSampler, reflect(gEyeDir.xyz,matInfo.worldNormal), 0).rgb;			
		
           	hdrOutput = float4(matInfo.color*diffuse.xyz+specular,1.0);//float4(matInfo.color*diffuse + specular,1.0);
	
        } 

		//for now write on all texels, but make sure to bail if stencil is not set.
		OutputBuffer[coords] = hdrOutput;
    }
}
