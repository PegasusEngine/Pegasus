#include "RenderSystems/3dTerrain/terrainCommon.h"

cbuffer BlockStateCbuffer
{
    float4 gWorldOffset;
    float4 gWorldScale;
};

Texture3D<float> densityTexture;
Texture3D<uint> meshCounts;
Texture3D<uint> meshSparse8;
Texture3D<uint> meshSparse4;

RWBuffer<uint> outIndexBuffer : u0;
RWByteAddressBuffer outVertexBuffer : u1;
RWByteAddressBuffer outNormalBuffer : u2;
RWByteAddressBuffer outIndirectBuffer : u3;
SamplerState densitySampler : s0;
#define DENSITY_DIMS float3(18,18,18)
groupshared uint gsmSparse4;
uint getGridVertexOffset(uint3 gridCoord)
{
	return dot(uint3(3,3*17,3*17*17), gridCoord);
}

float3 ComputeVertexNorm(int3 voxelPos)
{
	//gradient samples
	float2 xSamples = float2(densityTexture[voxelPos + int3(1,0,0)], densityTexture[voxelPos - int3(1,0,0)]);
	float2 ySamples = float2(densityTexture[voxelPos - int3(0,1,0)], densityTexture[voxelPos + int3(0,1,0)]);
	float2 zSamples = float2(densityTexture[voxelPos + int3(0,0,1)], densityTexture[voxelPos - int3(0,0,1)]);
	return normalize(-float3(xSamples.x - xSamples.y,ySamples.x - ySamples.y,zSamples.x - xSamples.y));  
	return float3(1.0,0.0,0.0);
}

float3 ComputeVertexNormBilinear(float3 voxelPos)
{
	float3 voxelUv = (voxelPos)/(DENSITY_DIMS - 1.0);

	//gradient samples
	float delta = 1.0/16.0;
	float2 xSamples = float2(densityTexture.SampleLevel(densitySampler,voxelUv - delta*float3(1,0,0), 0).x, densityTexture.SampleLevel(densitySampler,float3(voxelUv + delta*float3(1,0,0)), 0).x);
	float2 ySamples = float2(densityTexture.SampleLevel(densitySampler,voxelUv - delta*float3(0,1,0), 0).x, densityTexture.SampleLevel(densitySampler,float3(voxelUv + delta*float3(0,1,0)), 0).x);
	float2 zSamples = float2(densityTexture.SampleLevel(densitySampler,voxelUv + delta*float3(0,0,1), 0).x, densityTexture.SampleLevel(densitySampler,float3(voxelUv - delta*float3(0,0,1)), 0).x);
   	return normalize(-float3(xSamples.x - xSamples.y,ySamples.x - ySamples.y,zSamples.x - zSamples.y));

}

float3 ComputeVertexPos(uint edgeIndex, uint3 voxelPos)
{
	const int3 cubePos[8] = {
		int3(0, 0, 0), //0
		int3(1, 0, 0), //1
		int3(1, 1, 0), //2
		int3(0, 1, 0), //3
		int3(0, 0, 1), //4
		int3(1, 0, 1), //5
		int3(1, 1, 1), //6
		int3(0, 1, 1),  //7
	};
// 8e------    _~ 4----4e---5
//            0----0e----1 /|   ------9e
//            |   |      |  5e
//            3e  7e     1e |      
//            |   |      |  |
//            |_~ 7---6e-|--6
// 11e-----   3----2e----2/    -------10e
//

	//indices to vertexOffsets
	const uint2 edgeConnections[12] = {
		uint2(0, 1),
		uint2(1, 2),
		uint2(2, 3),
		uint2(3, 0),
		uint2(4, 5),
		uint2(5, 6),
		uint2(6, 7),
		uint2(7, 4),
		uint2(4, 0),
		uint2(5, 1),
		uint2(6, 2),
		uint2(7 ,3)
	};
	
	uint2 edgeConnection = edgeConnections[edgeIndex];
	int3 sampleOffsetA = cubePos[edgeConnection.x];
	int3 sampleOffsetB = cubePos[edgeConnection.y];

	int3 sampleCoordA = sampleOffsetA + voxelPos;
	int3 sampleCoordB = sampleOffsetB + voxelPos;
	float2 densitySamples = float2(densityTexture[sampleCoordA], densityTexture[sampleCoordB]); 
	float3 fCoordA = (float3(sampleOffsetA));
	float3 fCoordB = (float3(sampleOffsetB));	
	float midPoint = (MID_POINT - densitySamples.x) / (densitySamples.y - densitySamples.x);
	float3 coord = lerp(fCoordA,fCoordB, midPoint*float3(1,1,1));
	return coord + float3(voxelPos);
	
}

void WriteIndices(uint3 gridCoord, uint2 geoInfo, uint caseId)
{
	//remappings. Dependant on the fact of the ownership of each voxel on edges 0, 3 and 8
	const uint4 remapMask[12] = {
		uint4(0,0,0,0),//0
		uint4(1,0,0,1),//1
		uint4(0,1,0,0),//2
		uint4(0,0,0,1),//3
		uint4(0,0,1,0),//4
		uint4(1,0,1,1),//5
		uint4(0,1,1,0),//6
		uint4(0,0,1,1),//7
		uint4(0,0,0,2),//8
		uint4(1,0,0,2),//9
		uint4(1,1,0,2),//10
		uint4(0,1,0,2) //11
	};

	[loop]
	for (uint idxIt = 0; idxIt < geoInfo.y; ++idxIt) 
	{
		uint caseIndex = GetIndex(caseId, idxIt);
		uint4 remapMsk = remapMask[caseIndex];
		outIndexBuffer[geoInfo.x + idxIt] = getGridVertexOffset(gridCoord + remapMsk.xyz) + remapMsk.w ;
	} 
}

void BuildIndices(uint3 gi, uint3 gti, uint3 dti)
{
	uint3 gridCoord = gi * 8 + gti;
	uint countsInfo = meshCounts[gridCoord];  	
	uint2 indexCount_caseId = uint2(countsInfo >> 8, countsInfo & 0xff);
	uint caseId = indexCount_caseId.y;
    uint indexOffset = meshSparse8[gridCoord] - indexCount_caseId.x + gsmSparse4;
	uint2 geoInfo = uint2(indexOffset,indexCount_caseId.x);
    WriteIndices(gridCoord, geoInfo, caseId);
	
}

void WriteVertexes(uint voffset, uint3 voxelPos, uint3 gti, uint countInfo, uint caseId)
{
	uint byteOffset = voffset * 12; //three 32 bit components. Which is 12 bites. geoInfo.x contains offset in vertex buffer
	uint edgeOrder[] = {0, 3, 8};
	int edgeId = 0;
	uint3 gsmVert = voxelPos % 9;
	[loop]
	for (edgeId = 0; edgeId < 3; ++edgeId)
	{
		uint edgeIndex = edgeOrder[edgeId];
	    if (GetEdgeBit(countInfo, edgeIndex))
		{
			float3 vertexPos = ComputeVertexPos(edgeIndex, voxelPos)*gWorldScale.x + gWorldOffset.xyz;
			StoreFloat3Vertex(outVertexBuffer, byteOffset + 12*edgeId, vertexPos);
		}
	}

}

void WriteNormals(uint voffset, uint3 voxelPos, uint3 gti, uint countInfo, uint caseId)
{
	uint byteOffset = voffset * 12; //three 32 bit components. Which is 12 bites. geoInfo.x contains offset in vertex buffer
	uint edgeOrder[] = {0, 3, 8};
	int edgeId = 0;
	uint3 gsmVert = voxelPos % 9;

	[loop] 
	for (edgeId = 0; edgeId < 3; ++edgeId)
	{
		uint edgeIndex = edgeOrder[edgeId];
	    if (GetEdgeBit(countInfo, edgeIndex))
		{
			float3 vertexPos = ComputeVertexPos(edgeIndex, voxelPos);
			float3 outNormal = ComputeVertexNormBilinear(vertexPos);
			StoreFloat3Vertex(outNormalBuffer, byteOffset + 12*edgeId,outNormal);
		}
	}
}



void BuildVertexes(uint3 dti, uint3 gti)
{
	uint vOffset = getGridVertexOffset(dti);
	uint countsInfo = meshCounts[dti];  	
	uint2 indexCount_caseId = uint2(countsInfo >> 8, countsInfo & 0xff);
	uint caseId = indexCount_caseId.y;
	uint edgeCountInfo = GetCountInfo(caseId);
	WriteVertexes(vOffset, dti, gti, edgeCountInfo, caseId);
}

void BuildNormals(uint3 dti, uint3 gti)
{
	uint vOffset = getGridVertexOffset(dti);
	uint countsInfo = meshCounts[dti];  	
	uint2 indexCount_caseId = uint2(countsInfo >> 8, countsInfo & 0xff);
	uint caseId = indexCount_caseId.y;
	uint edgeCountInfo = GetCountInfo(caseId);
	WriteNormals(vOffset, dti, gti, edgeCountInfo, caseId);
}


[numthreads(9,9,9)]
void main(uint3 dti : SV_DispatchThreadId, uint3 gti : SV_GroupThreadId, uint3 gi : SV_GroupId)
{

    if (all(gti == 0))
    {
        gsmSparse4 = meshSparse4[gi];
    }

	GroupMemoryBarrierWithGroupSync();

	if (all(gti < 8))
	{
		BuildIndices(gi, gti, dti);
	}

	if (all(dti < 17))
	{
		BuildVertexes(dti, gti);
	}

	GroupMemoryBarrierWithGroupSync();

	if (all(dti < 17))
	{
		BuildNormals(dti, gti);
	}

	if (all(gi == 0) && all(gti == 0))
	{	

		uint totalCountIndices = meshSparse8[uint3(15,15,15)] + meshSparse4[uint3(1,1,1)];

  		//[in] UINT IndexCountPerInstance
		outIndirectBuffer.Store(0, totalCountIndices);

  		//[in] UINT InstanceCount
		outIndirectBuffer.Store(4,  1);

  		//[in] UINT StartIndexLocation,
		outIndirectBuffer.Store(8, 0);

  		//[in] INT  BaseVertexLocation,
		outIndirectBuffer.Store(12, 0);
 
  		//[in] UINT StartInstanceLocation
		outIndirectBuffer.Store(16, 0);

	}
}
