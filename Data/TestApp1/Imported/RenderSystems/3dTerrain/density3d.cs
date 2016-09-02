RWTexture3D<float> OutputVolume : register(u0);

#define T 3

float ComputeTerrainDensity(float3 samplePoint)
{
#if T == 0
	return 	saturate(samplePoint.x - 12.1);
#elif T == 1
	return 	saturate(samplePoint.x + 5.0*samplePoint.y*samplePoint.y - 12.1);
#elif T == 2
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05 + (rad > 0.42 ? 0.0 : 1.0));
#elif T == 3
	float rad = length(samplePoint.xy/8.0 - 1.0);
	return saturate(rad - 0.05);
#else
	float3 spacePoint = (samplePoint / 16.0)*2.0 - 1.0;
	return  max(saturate((samplePoint.y)+1.7 - pow((5.0*samplePoint.x/16),2.0)*1.0),saturate(length(spacePoint) < 1.0 ? 1.0 : 0.0));
#endif
}

[numthreads(9,9,9)]
void main(uint3 dti : SV_DispatchThreadId, uint3 gi : SV_GroupID, uint3 gti : SV_GroupThreadId)
{ 	
	float3 worldPos = dti;
	//worldPos.y = 16 - worldPos.y;
	OutputVolume[dti] = ComputeTerrainDensity(worldPos);
}
