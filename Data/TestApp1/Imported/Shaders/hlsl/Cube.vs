//
// Cube vs by Kleber Garcia 2014
//


cbuffer uniformState
{
    // place here all the uniforms to be used. Be sure to manually pad to a multiple of 16 bytes.
	float4x4 uTransform;
};

struct VS_OUT
{
	float4 p : POSITION;
	float2 t : TEXTURE0;
	float3 normal : NORMAL0;
};
VS_OUT  main(
	in float4 p0 : POSITION0,
	in float2 t0 : TEXCOORD0,
	in float3 n0 : NORMAL0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
    pos = mul(uTransform, p0);
    vo.p = pos;

    vo.t = t0.xy;
	pos *= 0.2;

	vo.normal = n0;
	return vo;

}


