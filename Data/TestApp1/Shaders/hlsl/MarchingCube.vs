#include "RenderSystems/Camera/Common.h"

struct VS_OUT
{
	float4 p : POSITION;	
	float3 n : NORMAL;
	float3 ccc : TEXCOORD0;
};

VS_OUT  main(
	in float4 p0 : POSITION0,
	in float3 n0 : NORMAL0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
	vo.ccc =  abs(p0.rgb);
	vo.p =  mul(p0, gView);
	vo.n = n0;
	pos = mul(vo.p,gProj);
	return vo;

}


