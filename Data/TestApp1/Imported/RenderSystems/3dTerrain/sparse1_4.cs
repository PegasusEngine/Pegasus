#include "RenderSystems/3dTerrain/terrainCommon.h"

RWTexture3D<uint> OutSparse : register(u0); 
Texture3D<uint> sparse8;

#define THREAD_COUNT (BLOCK_DIM/2)

// the 2 * THREAD_COUNT is to double buffer in a cache coherent way. Makes code more confusing, but more efficient
groupshared uint scratch[THREAD_COUNT][THREAD_COUNT][2*THREAD_COUNT];
groupshared uint scratch2[THREAD_COUNT][2*THREAD_COUNT];

[numthreads(THREAD_COUNT,THREAD_COUNT,THREAD_COUNT)]
void main(uint3 gti : SV_GroupThreadID)
{
	uint3 gti2 = gti * 2;

	//Step 1: move everything to group shared memory. Get the counts of the sparse8 groups. like a boss :D 	
	uint prevTarget = 0;
	uint nextTarget = 1;
	uint originalCount = sparse8[(gti + 1)*8 - 1];
	scratch[gti.z][gti.y][gti2.x + prevTarget] = originalCount;

	GroupMemoryBarrierWithGroupSync();

	// Step 1: X parallel reduction

	scratch[gti.z][gti.y][gti2.x + nextTarget] = scratch[gti.z][gti.y][gti2.x + prevTarget] + (gti.x > 0 ? scratch[gti.z][gti.y][gti2.x + prevTarget - 2] : 0);

	GroupMemoryBarrierWithGroupSync();

	// Step 2: Y parallel reduction
	if (gti.x == 1)
	{
		scratch2[gti.z][gti2.y + prevTarget] = scratch[gti.z][gti.y][gti2.x + nextTarget];
	}

	GroupMemoryBarrierWithGroupSync();	

	if (gti.x == 1)
	{
		scratch2[gti.z][gti2.y + nextTarget] = scratch2[gti.z][gti2.y + prevTarget] + ((gti.y > 0) ? scratch2[gti.z][gti2.y + prevTarget - 2] : 0);
	}

	GroupMemoryBarrierWithGroupSync();

	scratch[gti.z][gti.y][gti2.x + nextTarget] += gti.y > 0 ? scratch2[gti.z][gti2.y + nextTarget - 2] : 0; 


	// Step 3: Z parallel dispatch
	if (all(gti.xy == 1))
	{
		scratch2[0][gti2.z + nextTarget] = scratch[gti.z][gti.y][gti2.x + nextTarget];
	}

	GroupMemoryBarrierWithGroupSync();

	scratch2[0][gti2.z + prevTarget] = scratch2[0][gti2.z + nextTarget] + (all(gti.xy > 0) ? scratch2[0][gti2.z + nextTarget - 2] : 0);

	GroupMemoryBarrierWithGroupSync();

	//final resolve
	scratch[gti.z][gti.y][gti2.x + nextTarget] += gti.z > 0 ? scratch2[0][gti2.z + prevTarget - 2] : 0;
	scratch[gti.z][gti.y][gti2.x + nextTarget] -= originalCount;
	OutSparse[gti] = scratch[gti.z][gti.y][gti2.x + nextTarget];
}
