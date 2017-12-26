#include "RenderSystems/Volumes/VolumesCommon.h"

RWTexture3D<float> OutputVolume : register(u0);

cbuffer BlockStateCbuffer
{
    float4 gWorldOffset;
    float4 gWorldScale;
};

#define T 0

float ComputeTerrainDensity(float3 samplePoint)
{
#if T == 0
	samplePoint.y += 0.1*cos(0.5*samplePoint.x) + sin(0.05*samplePoint.z) - sin(0.1*(samplePoint.y*samplePoint.y-samplePoint.x)) + sin(0.02*samplePoint.z*samplePoint.z)*0.1;
	return 	3.0-samplePoint.y;
#elif T == 1
	return 	saturate(samplePoint.x + 5.0*samplePoint.y*samplePoint.y - 12.1);
#elif T == 2
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05 + (rad > 0.42 ? 0.0 : 1.0));
#elif T == 3
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05);
#elif T == 4
	samplePoint.x = fmod(samplePoint.x, 40.0);
	float sq = all(samplePoint > float3(0.0,0.0,0.0) && samplePoint < float3(16.0,16.0,16.0));
	return lerp(0.0, 1.0, sq); 
#else

	float sphere = length(samplePoint/8.0 - 1.0);
	return saturate(sphere/30);
#endif
}

[numthreads(BLOCK_DIM+2,BLOCK_DIM+2,BLOCK_DIM+2)]
void main(uint3 dti : SV_DispatchThreadId, uint3 gi : SV_GroupID, uint3 gti : SV_GroupThreadId)
{ 	
	if (all(dti < uint3(THREAD_DIM + 3, THREAD_DIM + 3, THREAD_DIM + 3)))
	{
		float3 worldPos = dti*gWorldScale.x + gWorldOffset.xyz;
		OutputVolume[dti] = gWorldScale.x*ComputeTerrainDensity(worldPos - float3(1.0,3.0,1.0));
	}
}
