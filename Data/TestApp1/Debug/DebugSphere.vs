#include "RenderSystems/Camera/Common.h"

//
// DebugSphere vs by Kleber Garcia 2014
//


cbuffer uniformState
{
	float4x4 uTransform;
	float4x4 uNormTransform;
};

struct InstanceData
{
	float3 translation;
	float2 reflectanceSmoothness;
};

StructuredBuffer<InstanceData> instanceDataBuffer;

struct VS_OUT
{
	float4 p : POSITION;
	float3 normal : NORMAL0;
	float2 t : TEXTURE0;
	float2 matInfo : TEXTURE1;

};
VS_OUT  main(
	in float4 p0 : POSITION0,
	in float2 t0 : TEXCOORD0,
	in float3 n0 : NORMAL0,
	in uint instanceId : SV_InstanceId,
	out float4 pos : SV_Position
)
{
	VS_OUT vo; 
//	float3 instancePos = float3(1.0,1.0,1.0);//instancePos[0];
	InstanceData instanceDataSample = instanceDataBuffer[instanceId];
    vo.p = mul(p0, uTransform) + float4(instanceDataSample.translation,0);
	vo.matInfo = instanceDataSample.reflectanceSmoothness;
	pos = mul(vo.p,gViewProj);
    vo.t = t0.xy;
	
	vo.normal = mul(float4(n0,0.0f), uNormTransform).xyz;
	return vo;

}