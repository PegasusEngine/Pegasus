#include "RenderSystems/Camera/Common.h"

cbuffer uniformState
{
	float4x4 uWorld;
	float4x4 uProj;
};

struct VS_OUT
{
	float4 p : POSITION;
	float4 localPos : TEXTURE0;
	float3 normal : NORMAL0;
	float2 t : TEXTURE1;


};

VS_OUT  main(
	in float4 p0 : POSITION0,
	in float2 t0 : TEXCOORD0,
	in float3 n0 : NORMAL0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
	float4x4 worldView = mul(uWorld,gView);
	vo.localPos = mul(p0,uWorld); 
	vo.p =  mul(p0, worldView);
	vo.normal = n0;
	vo.t = t0;
	pos = mul(vo.p,gProj);
	pos.z = pos.z;
	return vo;

}


