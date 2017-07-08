//*******  Lighting Core file             ************//
// Uber shader for lighting components                //
//****************************************************//

#ifndef USE_DEFERRED_RENDERER
#define USE_DEFERRED_RENDERER 1
#endif

#include "RenderSystems/Lighting/GBuffer.h"
#include "RenderSystems/Lighting/Brdf.h"

//basic light information
struct LightInfo 
{
	float4 attr0;
	float4 attr1;
	float4 attr2;
	int2   attr3;
};

uint GetLightInfoType(in LightInfo info)
{
	return info.attr3.x;
}

//light types
struct SphereLight
{
	float4 colorAndIntensity;
	float4 posAndRadius;
};

struct SpotLight
{
	float4 colorAndIntensity;
	float4 posAndRadius;
	float4 dirAndAngle;
};

//casting function
void GetSphereLight(in LightInfo info, out SphereLight light)
{
	light.colorAndIntensity = info.attr0;
	light.posAndRadius = info.attr1;
}

void GetSpotLight(in LightInfo info, out SpotLight light)
{
	light.colorAndIntensity = info.attr0;
	light.posAndRadius = info.attr1;
	light.dirAndAngle = info.attr2; 
}

//application of lights

float LightDistanceAttenuation(float distanceToLight, float lightRadius)
{
	float normalizedDistance = distanceToLight/max(lightRadius,0.00001);
	return saturate(1.0 / (normalizedDistance*normalizedDistance+0.00001));
}

void ApplySphereLight(in float3 worldPos, in MaterialInfo material, in float3 viewVector, in SphereLight light, in out float3 diffuse, in out float3 specular)
{
	float3 L = light.posAndRadius.xyz-worldPos;
	float distanceToLight = length(L);
	L /= distanceToLight;
	float3 intensity = light.colorAndIntensity.rgb*LightDistanceAttenuation(distanceToLight, light.posAndRadius.w);
	intensity *= light.colorAndIntensity.xyz*light.colorAndIntensity.w;

	float irradiance = saturate(dot(material.worldNormal,L));
  
	diffuse += irradiance*intensity;

	BrdfInfo brdfInfo;
	GetBrdfInfo(brdfInfo, viewVector, L, material.worldNormal, material.smoothness, material.reflectance);
	specular += irradiance*ComputeSpecular(intensity,brdfInfo);
}

void ApplySpotLight(in float3 worldPos, in MaterialInfo material, in float3 viewVector, in SpotLight light, in out float3 diffuse, in out float3 specular)
{
	float3 L = light.posAndRadius.xyz-worldPos;
	float distanceToLight = length(L);
	L /= distanceToLight;
	float3 intensity = light.colorAndIntensity.rgb*light.colorAndIntensity.w*LightDistanceAttenuation(distanceToLight, light.posAndRadius.w);
	float NdL = saturate(dot(material.worldNormal,L));
	float irradiance = NdL;

	float angleDot = saturate(dot(L,-normalize(light.dirAndAngle.xyz)));
 
	//TODO: precompute the max angle in the cpu
	float h = sqrt(light.posAndRadius.w*light.posAndRadius.w+light.dirAndAngle.w*light.dirAndAngle.w);
	float maxAngle = light.posAndRadius.w/h;
	float angleDiff = max(1.0-maxAngle,0.0001);
	float angleAttenuation = saturate(1.0-(1.0-angleDot)/angleDiff);
 	irradiance *= angleAttenuation;

	diffuse += irradiance*intensity;
	BrdfInfo brdfInfo;
	GetBrdfInfo(brdfInfo, viewVector, L, material.worldNormal, material.smoothness, material.reflectance);
	specular += irradiance*ComputeSpecular(intensity,brdfInfo);

}

// Light type definitions that must match C++'s LightRig.h LightType enum
#define LIGHTTYPE_SPHERE 0
#define LIGHTTYPE_SPOT 1
 
#if USE_DEFERRED_RENDERER
#define LightingOutput GBuffer
#else
struct LightingOutput
{
    float4 out : SV_Target0;
};
#endif



LightingOutput ApplyLighting(in MaterialInfo matInfo)
{
#if USE_DEFERRED_RENDERER
    return WriteToGBuffers(matInfo);
#else
    LightingOutput output;
    output.out = ForwardLighting(matInfo);
    return output;
#endif
}

