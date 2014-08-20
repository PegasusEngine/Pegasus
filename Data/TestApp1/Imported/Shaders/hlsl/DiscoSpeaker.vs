//
// DiscoSpeaker vs, by Kleber Garcia 2014
//
struct VS_OUT
{
	float4 p : POSITION0;
};
VS_OUT main(
	in float4 p0 : POSITION0,
	out float4 pos : SV_POSITION
)
{
	VS_OUT vo;
    pos=float4(p0.x,p0.y,0.01,1.0);
	vo.p = pos;
	return vo;
}



