cbuffer uniformState
{
float screenRatio;
float time;
}
struct VS_OUT
{
	float3 p : TEXCOORD1;
	float3 d : TEXCOORD0;
};
VS_OUT main(
	in float2 p0 : POSITION0,
	out float4 pos : SV_Position
){
	VS_OUT vo;
    pos=float4(p0.x,p0.y*screenRatio,0.0,1.0);
    vo.p = float3(0,0,time*0.15);
    vo.d = float3(p0.x * cos(time*0.25) + sin(time*0.25), p0.y + .4*sin(time*0.25), cos(time*0.25) - p0.x * sin(time*0.25));
	return vo;
}

void main(
	in float3 p : TEXCOORD1,
	in float3 d : TEXCOORD0,
	out float4 color : SV_Target
)
{
	float3 q,r;
	float f=0,g=0,k=0,c=0,t=0;
	int i=0,j;
	do
	{
		f=-1;
		k=1;
		for(j=0;j<6;++j)
		{
			q = ((1/6.) - abs(frac((d*t + p) * k + .5) - .5)) / k;
			r = min(q,q.yzx);
			g = max(r.x, max(r.y,r.z));
			if(g>f)
			{
				f=g;
				c=d.x;
				if(g==q.y) c=d.y;
				if(g==q.z) c=d.z;
			}
			/*k*=3;*/
			/*k*=2;*/
            /*k*=1.73;*/
            k*=1.7;
		}
		t+=.7*f;
		i++;
	}
	while(f>.0006 && i<100);
    color = abs(c) / (1+t) / (1.+float(i)*.05);
}
