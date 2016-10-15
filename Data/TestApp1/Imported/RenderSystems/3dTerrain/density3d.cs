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
	samplePoint.y += cos(0.5*samplePoint.x) + sin(0.5*samplePoint.z) - sin(samplePoint.y*samplePoint.y-samplePoint.x) + sin(samplePoint.z*samplePoint.z);
	return 	3.0-samplePoint.y;
#elif T == 1
	return 	saturate(samplePoint.x + 5.0*samplePoint.y*samplePoint.y - 12.1);
#elif T == 2
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05 + (rad > 0.42 ? 0.0 : 1.0));
#elif T == 3
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05);
#else

	float sphere = length(samplePoint/8.0 - 1.0);
	return saturate(sphere/30);
#endif
}

[numthreads(9,9,9)]
void main(uint3 dti : SV_DispatchThreadId, uint3 gi : SV_GroupID, uint3 gti : SV_GroupThreadId)
{ 	
	float3 worldPos = dti*gWorldScale.x + gWorldOffset.xyz;
	OutputVolume[dti] = gWorldScale.x*ComputeTerrainDensity(worldPos);
}
