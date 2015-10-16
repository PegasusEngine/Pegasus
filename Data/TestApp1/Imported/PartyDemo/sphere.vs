cbuffer uniformState
{
	float4x4 uWorld;
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
	vo.p =  mul(p0, uWorld);
	vo.normal = n0;//mul(float4(n0,0.0),uWorld).xyz;
	vo.t = t0;
	pos = mul(vo.p,uProj);
	return vo;

}


