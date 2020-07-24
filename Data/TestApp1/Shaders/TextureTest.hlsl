Texture2D inputTexture;

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
    pos = float4(p0.x * 0.9 / screenRatio, p0.y * 0.9, 0.0, 1.0);
	vo.texCoords = p0.xy * 0.5 + 0.5;
	return vo;
}

SamplerState S
{
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

void main(
	in float2 texCoords : TEXCOORD0,
	out float4 color : SV_Target
)
{
    color = inputTexture.Sample(S, texCoords);
}
