#include "RenderSystems/Camera/Common.h"
#include "RenderSystems/Lighting/LightingCore.h"

cbuffer uniformState
{
	float4x4 uTransform;
	float4x4 uNormTransform;
};

struct VS_OUT
{
	float4 p : POSITION;
	float3 normal : NORMAL0;
	float2 t : TEXTURE0;

};

VS_OUT  main(
	in float4 p0 : POSITION0,
	in float2 t0 : TEXCOORD0,
	in float3 n0 : NORMAL0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
    vo.p = mul(p0, uTransform);
	
	pos = mul(vo.p,gViewProj);
    vo.t = t0.xy;
	
	vo.normal = mul(float4(n0,0.0f), uNormTransform).xyz;
	return vo;

}


float hash(float3 p)
{
    p  = frac( p*0.3183099+.1 );
	p *= 17.0;
    return frac( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float noise( float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
	
    return lerp(lerp(lerp( hash(p+float3(0,0,0)), 
                        hash(p+float3(1,0,0)),f.x),
                   lerp( hash(p+float3(0,1,0)), 
                        hash(p+float3(1,1,0)),f.x),f.y),
               lerp(lerp( hash(p+float3(0,0,1)), 
                        hash(p+float3(1,0,1)),f.x),
                   lerp( hash(p+float3(0,1,1)), 
                        hash(p+float3(1,1,1)),f.x),f.y),f.z);
}

float proceduralConcrete(float3 p)
{
	float o1 = noise(p);
	float o2 = noise(p*2.0)*0.7;
	float o3 = noise(p*4.0)*0.4;
	float s = saturate(0.4*(o1 + o2 + o3));
	return pow(s,1);
}

void main(
	in float4 p : POSITION0,
	in float3 normal : NORMAL0,
	in float2 t : TEXTURE0,
	out LightingOutput lightingOut
)
{
	float concreteNoise = proceduralConcrete(1.0*p.xyz);
	float3 nNormal = normalize(normal+0.21*float3(0.0,1.0,0.0)*concreteNoise);
	MaterialInfo material;
	material.color = float3(0.4,0.4,0.4);
	material.worldNormal = nNormal;
	material.smoothness = concreteNoise;
	material.reflectance = 0.05;
	material.metalMask = 0.0;
	lightingOut = ApplyLighting(material);

}
