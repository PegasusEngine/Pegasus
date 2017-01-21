#ifndef TERRAIN_COMMON_SH
#define TERRAIN_COMMON_SH

#define MID_POINT 0.5

// 8e------    _~ 4----4e---5
//            0----0e----1 /|   ------9e
//            |   |      |  5e
//            3e  7e     1e |      
//            |   |      |  |
//            |_~ 7---6e-|--6
// 11e-----   3----2e----2/    -------10e

// Enumerations that must match those from Terrain3dSystem::Constants

//number of cases in one dimension.
#define BLOCK_DIM  8 
//number of blocks on a single dispatch
#define GROUP_DIM 2

//cases dont exceed 5 triangles
#define MAX_TRI_PER_CASE 5 

#define BLOCK_SIZE (BLOCK_DIM * BLOCK_DIM * BLOCK_DIM)
#define THREAD_DIM  (BLOCK_DIM * GROUP_DIM)
#define VERTEX_SIZE  ((THREAD_DIM+1) * (THREAD_DIM+1) * (THREAD_DIM+1) * 3) //3 roots
#define INDEX_SIZE  (GROUP_DIM * GROUP_DIM * GROUP_DIM * BLOCK_SIZE * MAX_TRI_PER_CASE * 3)



cbuffer countInfoCbuffer
{
	uint4 countInfo[64];
}

cbuffer indexInfoCbuffer
{
	uint4 indexInfo[256*4];
}

uint GetCountInfo(uint caseId)
{
	return countInfo[caseId/4][caseId%4];
}

uint GetEdgeBit(uint countInfo, uint edgeId)
{
	return 0x1 & (countInfo >> edgeId);
}

uint GetIndex(uint caseId, uint i)
{
	return indexInfo[caseId*4 + i/4][i % 4];
}


void StoreFloat3Vertex(RWByteAddressBuffer buffer, uint byteOffset, float3 pos)
{
	buffer.Store3(byteOffset, 
		uint3(asuint(pos.x), asuint(pos.y), asuint(pos.z)));
}


#endif
