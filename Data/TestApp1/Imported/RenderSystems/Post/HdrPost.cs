//*******  HdrPost, post effects in hdr that will go into LUT / Tonemapping ***//
//                                                                             //
//*****************************************************************************//


Texture2D<float4> HdrSceneIn;
RWTexture2D<float4> OutputLdrTarget : register(u0);

cbuffer Constants
{
    uint2 gTargetDimensions;
	float gExposureVal;
	float pad0;
};

float3 ACESFilm( float3 x )
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float4 ToneMap(float4 hdrColor)
{
	//todo: implement tone mapping, for now just linearizing
	return float4(ACESFilm(gExposureVal*hdrColor.rgb),hdrColor.a);
}

#define TILE_SIZE 32
[numthreads(TILE_SIZE,TILE_SIZE,1)]
void main(uint3 gti : SV_DispatchThreadId)
{
	if (all(gti.xy < gTargetDimensions))
	{
		float4 hdrColor = HdrSceneIn[gti.xy];
		float4 ldrColor = ToneMap(hdrColor);
		OutputLdrTarget[gti.xy] = ldrColor;
	}
}