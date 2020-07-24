
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
	vo.p.y += 0.2 * pow(1.0-frac(time), 4.0);
    vo.d=float3(p0.x*vo.p.x-p0.y*vo.p.y,p0.y*vo.p.x+p0.x*vo.p.y,1);
	return vo;
}

void main(
	in float3 p : TEXTURE0,
	in float3 d : TEXTURE1,
	out float4 color : SV_Target
)
{
    float3 e=normalize(d),r;
    float f,t=0;
    int i=0;
    do
    {
        r=frac(e*t+float3(p.xy,0))-.5;
        f=min(.25*(length(r.yz)+length(r.xy)),length(r.xz))-.1;
        t+=.5*f;
		i++;
    }
    while(f>.01&&i<100);
    f=sin(t);
    color=float4(f*.5+.2+smoothstep(.6,.62,i*.01)*smoothstep(.64,.62,i*.01),f*.6+.2,f*.7+.3,1);

	color.xyz *= 1.0 / (1.0 + frac(4.0 * p.z / 3.0));
}
