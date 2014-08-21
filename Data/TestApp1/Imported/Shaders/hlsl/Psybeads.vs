//
//Psybeads vs, by Kevin Boulanger 2013
//translated to hlsl by Kleber Garcia 2014
//

cbuffer uniformState
{
float screenRatio;
float time ;

};
struct VS_OUT
{

    float3 p : TEXTURE0;
	float3 d : TEXTURE1;
};
VS_OUT main(
    in float2 p0 : POSITION0,
    out float4 pos : SV_Position
){
	VS_OUT vo;
    pos=float4(p0.x,p0.y*screenRatio,0.0,1.0);
    vo.p=3.0*float3(cos(time*0.25),sin(time*0.25),time*0.25);
    vo.d=float3(p0.x*vo.p.x-p0.y*vo.p.y,p0.y*vo.p.x+p0.x*vo.p.y,1);
	return vo;
}
