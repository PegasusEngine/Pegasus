// Test for dx12 shader.

Texture2D<float4> inputTexture : register(t0);
SamplerState bilinearSampler : register(s0);

cbuffer Constants : register(b0)
{
    float4 val1;
    float4 val2;
}

void vsMain(in float2 p0 : POSITION0, out float4 pos : SV_Position, out float2 texCoords : TEXCOORD0)
{
    texCoords.xy = float2(p0.xy * 0.5 + 0.5) + val1.xy;
    pos = float4(p0.xy, 0.0, 1.0);
}

float4 psMain(in float2 texCoords : TEXCOORD0) : SV_Target
{
    return inputTexture.Sample(bilinearSampler, texCoords);
}
