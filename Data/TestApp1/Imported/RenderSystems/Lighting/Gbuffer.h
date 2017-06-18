//*******  GBUFFER UTILITY                *******//
// Utilities for gbuffer                         //
//***********************************************//

#ifndef GBUFFER_H
#define GBUFFER_H

struct MaterialInfo
{
	float3 color;
	float3 worldNormal;
	float  smoothness;
	float  reflectance;
	float  metalMask;
};

struct GBuffer
{
	float4 gbuffer0 : SV_Target0;
	float4 gbuffer1 : SV_Target1;
};

GBuffer WriteToGBuffers(in MaterialInfo info)
{
	GBuffer gbuffer;
	gbuffer.gbuffer0 = float4(info.color.rgb,info.smoothness);
	
	uint reflectanceUInt = uint(info.reflectance * 255);
	uint metalMaskUInt = uint(info.metalMask * 255);	
	gbuffer.gbuffer1 = float4(info.worldNormal.xyz, asfloat( (reflectanceUInt << 8) | ( metalMaskUint ) ));

	return gbuffer;
}

MaterialInfo ReadFromGbuffers(in GBuffer gbuffer)
{
	MaterialInfo info;

	info.color = gbuffer.gbuffer0.rgb;
	info.smoothness = gbuffer.gbuffer0.a;
	info.worldNormal = gbuffer.gbuffer1.xyz;
	
	uint alphaAsUint = asuint(gbuffer.gbuffer1.a);
	info.metalMask = float(alphaAsUint & 0xFF)/255;
	info.reflectance = float((alphaAsUint >> 8) & 0xFF)/255;

	return info;
}

#endif
