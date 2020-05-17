#include "RenderSystems/Camera/Common.h"

//
// Cube vs by Kleber Garcia 2014
//


cbuffer uniformState
{
    // place here all the uniforms to be used. Be sure to manually pad to a multiple of 16 bytes.
	float4x4 uTransform;
	float4x4 uProj;
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
	float4x4 worldView = mul(uTransform,gView);
    vo.p = mul(p0, worldView);
	
	pos = mul(vo.p,gProj);
    vo.t = t0.xy;
	
	//dx11 the y axis is inverted
	vo.normal = n0;
	return vo;

}


