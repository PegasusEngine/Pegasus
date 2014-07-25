//
//Cubefractal2 vs, by Kevin Boulanger 2013
//translated to hlsl by Kleber Garcia 2014
//


float time : register(c0);
float screenRatio : register(c1);
void main(
	in float2 p0 : POSITION0,
	out float4 pos : SV_Position,
	out float3 p : TEXCOORD1,
	out float3 d : TEXCOORD0
){
    pos=float4(p0.x,p0.y*screenRatio,0.0,1.0);
    p = float3(0,0,time*.6);
    d = float3(p0.x * cos(time) + sin(time), p0.y + .4*sin(time), cos(time) - p0.x * sin(time));
}
