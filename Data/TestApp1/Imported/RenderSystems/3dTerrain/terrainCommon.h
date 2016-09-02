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
//

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
