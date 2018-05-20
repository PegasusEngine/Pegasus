#include <RenderSystems/Lighting/ShUtil.h>
#include <RenderSystems/Sampling.h>

cbuffer Constants
{
	int numSamples;
	float3 padding;
};

TextureCube<float4> InputCube;
RWStructuredBuffer<ShL2> output : register(u0);
SamplerState CubeSampler : register(s0);

[numthreads(1,1,1)]
void main()
{
	ShL2 shResult;
	[unroll]
	for (uint b = 0; b < 9; ++b) shResult.v[b] = float4(0,0,0,0);
	uint usNumSamples = (uint)numSamples;

	[loop]
	for (uint it = 0; it < usNumSamples; ++it)
	{
		float2 uv = hammersley2d(it, usNumSamples);
		float3 normal = sphereSample_uniform(uv);
		ShL2 b = L2Basis(normal);
		float4 L = InputCube.SampleLevel(CubeSampler, normal, 0);
		shMultiply(b, L);
		shAdd(shResult, b);		
	} 
	shMultiply(shResult, (1.0/float(numSamples)).xxxx);
	output[0] = shResult; 
}