//*************************************************************************//
//                        Full screen vertex shader. 
//*************************************************************************//

//
// TextureTest vs, by Kevin Boulanger 2014
// translated to hlsl by Kleber Garcia 2014
//

cbuffer uniformState
{
	float screenRatio;
}
struct VS_OUT
{
	float2 texCoords : TEXCOORD0;	
};
VS_OUT main(
	in float2 p0 : POSITION0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
    pos = float4(p0.x, p0.y, 0.0, 1.0);
	vo.texCoords = p0.xy * float2(0.5,-0.5) + 0.5;;
	return vo;
}
