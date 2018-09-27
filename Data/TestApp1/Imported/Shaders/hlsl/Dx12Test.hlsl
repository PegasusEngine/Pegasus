// Test for dx12 shader.

Texture2D<float4> inputTexture  : register(t0);
Texture2D<float4> inputTexture1 : register(t1);
Texture2D<float4> inputTexture2 : register(t2);
Texture2D<float4> inputTexture3 : register(t3);
Texture2D<float4> inputTexture4[3] : register(t4);
SamplerState bilinearSampler : register(s0);

cbuffer Constants : register(b0)
{
    float4 val1;
    float4 val2;
}

struct VS_OUT
{
    float4 texCoords : TEXCOORD0;
    float4 pos : SV_Position;
};

VS_OUT vsMain(in float2 p0 : POSITION0)
{
    VS_OUT output;
    float4 texSamplVal = inputTexture2.SampleLevel(bilinearSampler, float2(0,0), 0);
    output.texCoords = float4(float2(p0.xy * 0.5 + 0.5) + texSamplVal.xy, 0.0, 0.0);
    output.pos = float4(p0.xy, 0.0, 1.0);
    return output;
}

float4 psMain(VS_OUT psIn) : SV_Target
{
    float4 texCoords = psIn.texCoords;
    return inputTexture.Sample(bilinearSampler, texCoords.xy) + inputTexture3.Sample(bilinearSampler, texCoords.xy) + inputTexture4[8].Sample(bilinearSampler, texCoords.xy);
}
