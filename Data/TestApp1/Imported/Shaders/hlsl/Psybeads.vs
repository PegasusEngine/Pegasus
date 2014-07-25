//
//Psybeads vs, by Kevin Boulanger 2013
//translated to hlsl by Kleber Garcia 2014
//


float time : register(c0);
float screenRatio : register(c1);

void main(
    in float2 p0 : POSITION0,
    out float4 pos : SV_Position,
    out float3 p : TEXTURE0,
	out float3 d : TEXTURE1
){
    pos=float4(p0.x,p0.y*screenRatio,0.0,1.0);
    p=3.0*float3(cos(time),sin(time),time);
    d=float3(p0.x*p.x-p0.y*p.y,p0.y*p.x+p0.x*p.y,1);
}
