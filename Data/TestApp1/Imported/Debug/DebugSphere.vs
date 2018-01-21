#include "RenderSystems/Camera/Common.h"

//
// DebugSphere vs by Kleber Garcia 2014
//


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