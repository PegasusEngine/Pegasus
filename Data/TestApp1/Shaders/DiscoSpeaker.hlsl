cbuffer uniformState
{
float time;
float glitteIntensity;
float2 glitterRepetition;
}

struct VS_OUT
{
	float4 p : POSITION0;
};
VS_OUT main(
	in float4 p0 : POSITION0,
	out float4 pos : SV_POSITION
)
{
	VS_OUT vo;
    pos=float4(p0.x,p0.y,0.01,1.0);
	vo.p = pos;
	return vo;
}

static const float E = 2.7182818284;

void main(
	in float4 p : POSITION0,
	out float4 color : SV_Target
)
{
	float2 circle = p.xy;

	circle *= circle;
	float pulse1 = clamp(frac(time)*3.0,1.0,1.6);
	float c = dot(circle, float2(1.0,1.0))*pulse1;
	float rad = clamp(50.0*c,0.0,40.0);
 
	float outsideDisplacement = pow(E, sin(rad));
	float internalDisplacement = clamp((1.0 - outsideDisplacement),0.0,1.0);
	float glitter = glitteIntensity*(cos(p.x*glitterRepetition.x+time*10.4)+sin(p.y*glitterRepetition.y+time*10.5));
		
    color = float4(outsideDisplacement+glitter,internalDisplacement+glitter,0.0,1.0);
} 