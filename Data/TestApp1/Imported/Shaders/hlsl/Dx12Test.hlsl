// Test for dx12 shader.

Texture2D<float4> inputTexture  : register(t0);
Texture2D<float4> inputTexture1[16] : register(t1);
Texture2D<float4> inputTexture2 : register(t17);
Texture2D<float4> inputTexture3 : register(t18);
Texture2D<float4> inputTexture4 : register(t19);
SamplerState bilinearSampler : register(s0);

cbuffer Constants : register(b0)
{
    float4 val1;
    float4 val2;
}

void vsMain(in float2 p0 : POSITION0, out float4 pos : SV_Position, out float2 texCoords : TEXCOORD0)
{
    float4 texSamplVal = inputTexture2.SampleLevel(bilinearSampler, float2(0,0), 0);
    texCoords.xy = float2(p0.xy * 0.5 + 0.5) + texSamplVal.xy;
    pos = float4(p0.xy, 0.0, 1.0);
}

float4 psMain(in float2 texCoords : TEXCOORD0) : SV_Target
{
    return inputTexture.Sample(bilinearSampler, texCoords) + inputTexture1[8].Sample(bilinearSampler, texCoords) + inputTexture4.Sample(bilinearSampler, texCoords);
}
