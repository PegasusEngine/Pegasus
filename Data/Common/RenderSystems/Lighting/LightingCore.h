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
	float innerRadius;
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
	light.innerRadius = info.attr2.x;
}

void GetSpotLight(in LightInfo info, out SpotLight light)
{
	light.colorAndIntensity = info.attr0;
	light.posAndRadius = info.attr1;
	light.dirAndAngle = info.attr2; 
}

//application of lights
float LightDistanceAttenuation(float distToLightSquared, float lightRadius)
{
	float distAttenuation = saturate(1.0 / max(distToLightSquared,0.001));
	float smoothRadialAttenuation = 1.0 - saturate(distToLightSquared/(lightRadius*lightRadius));
	return distAttenuation*smoothRadialAttenuation*smoothRadialAttenuation ;
}

void ApplySphereLight(in float3 worldPos, in MaterialInfo material, in float3 viewVector, in SphereLight light, in out float3 diffuse, in out float3 specular)
{
	const float MAX_SPHERE_LIGHT_DIST = 10000000.0;
	float3 uL = light.posAndRadius.xyz-worldPos;
	float distL = clamp(length(uL)-light.innerRadius,0.01,MAX_SPHERE_LIGHT_DIST);
	float distSqrd = distL*distL;
	float3 L = uL/distL;
	float newLightRadius = clamp(light.posAndRadius.w-light.innerRadius,0.0,MAX_SPHERE_LIGHT_DIST);

	float3 intensity = light.colorAndIntensity.rgb*light.colorAndIntensity.w*LightDistanceAttenuation(distSqrd,newLightRadius);
	float irradiance = saturate(dot(material.worldNormal,L));
  
	diffuse += irradiance*intensity;

	BrdfInfo brdfInfo; 
	GetBrdfInfo(brdfInfo, viewVector, L, material.worldNormal, material.smoothness, material.reflectance);
	specular += irradiance*ComputeSpecular(intensity,brdfInfo);
}

void ApplySpotLight(in float3 worldPos, in MaterialInfo material, in float3 viewVector, in SpotLight light, in out float3 diffuse, in out float3 specular)
{
	const float MAX_SPOT_LIGHT_DIST = 10000000.0;
	float3 uL = light.posAndRadius.xyz-worldPos;
	float distL = length(uL);
	float distSqrd = distL*distL;
	float3 L = uL/distL;

	float3 intensity = light.colorAndIntensity.rgb*light.colorAndIntensity.w*LightDistanceAttenuation(distSqrd,light.posAndRadius.w);
	float irradiance = saturate(dot(material.worldNormal,L));

	float angleDot = saturate(dot(L,-normalize(light.dirAndAngle.xyz))); 
	//TODO: precompute the max angle in the cpu
	float h = sqrt(light.posAndRadius.w*light.posAndRadius.w+light.dirAndAngle.w*light.dirAndAngle.w);
	float maxAngle = light.posAndRadius.w/h;
	float angleDiff = max(1.0-maxAngle,0.0001);
	float invAngleAttenuation = (1.0-angleDot)/angleDiff;
	float angleAttenuation = saturate(1.0-invAngleAttenuation*invAngleAttenuation);
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

