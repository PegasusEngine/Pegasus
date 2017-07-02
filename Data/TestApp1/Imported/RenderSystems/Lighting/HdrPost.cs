//*******  HdrPost, post effects in hdr that will go into LUT / Tonemapping ***//
//                                                                             //
//*****************************************************************************//


Texture2D<float4> HdrSceneIn;
RWTexture2D<float4> OutputLdrTarget;

cbuffer Constants
{
    uint2 gTargetDimensions;
	float2 padding;
};

float4 ToneMap(float4 hdrColor)
{
	//todo: implement tone mapping, for now just linearizing
	return hdrColor / 4.0;
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