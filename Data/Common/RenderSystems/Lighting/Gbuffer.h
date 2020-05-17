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
	
	float reflectanceRange = floor(saturate(info.reflectance) * 255.0);
	float metalMaskRange = floor(saturate(info.metalMask) * 255.0) * 0.001;	
	gbuffer.gbuffer1 = float4(info.worldNormal.xyz, reflectanceRange+metalMaskRange);

	return gbuffer;
}

MaterialInfo ReadFromGbuffers(in GBuffer gbuffer)
{
	MaterialInfo info;

	info.color = gbuffer.gbuffer0.rgb;
	info.smoothness = gbuffer.gbuffer0.a;
	info.worldNormal = gbuffer.gbuffer1.xyz;	
	info.metalMask = frac(gbuffer.gbuffer1.a)*1000.0/255.0;
	info.reflectance = floor(gbuffer.gbuffer1.a)/255.0;

	return info;
}

#endif
