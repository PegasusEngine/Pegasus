#include "RenderSystems/3dTerrain/terrainCommon.h"

RWTexture3D<uint> OutVolumeInfo : register(u0);
Texture3D<float> densityTexture : register(t0);

groupshared float densitySamples[10][10][10];

uint ComputeIndexCount(uint caseId, uint3 gti)
{
	return (GetCountInfo(caseId) >> 16) * 3; //packed triangle count * 3
}

[numthreads(10,10,10)]
void main(uint3 dti : SV_DispatchThreadId, uint3 gti : SV_GroupThreadId, uint3 gi : SV_GroupId)
{ 	
	uint3 coord = 8*gi + gti;
	densitySamples[gti.x][gti.y][gti.z] = densityTexture[coord];
	
	GroupMemoryBarrierWithGroupSync();

	//sample
	const uint3 sampleCoords[8] = {
		uint3(0,0,0),
		uint3(1,0,0),
		uint3(1,1,0),
		uint3(0,1,0),
		uint3(0,0,1),
		uint3(1,0,1),
		uint3(1,1,1),
		uint3(0,1,1)
	};


	uint caseId = 0;
	uint3 maxMask = uint3(8,8,8) + (gi >= uint3(1,1,1));
	if (all(gti < maxMask))
	{
		[unroll]
		for (uint s = 0; s < 8; ++s)
		{
			uint3 sampleCoord = gti + sampleCoords[s];
			float samp =  densitySamples[sampleCoord.x][sampleCoord.y][sampleCoord.z];
			caseId = caseId | ((samp > MID_POINT ? 0x1 : 0x0) << s);
		}
		uint3 outputCoord = 8*gi + gti;
		uint indexCount = ComputeIndexCount(caseId, gti);
		OutVolumeInfo[outputCoord] = (indexCount << 8) | (0xff & caseId); 
	}

}
