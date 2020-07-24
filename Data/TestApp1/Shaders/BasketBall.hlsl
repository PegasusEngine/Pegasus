
#include "RenderSystems/Camera/Common.h"

cbuffer pixelUniformState
{
	float4 uCameraPos_beat;
	float4 uCenterPos_unused;
	float4 lightPosition;
	float4 worldCamera;
	float4 stripThickness_AA_unused;
};

cbuffer lightingUniformState
{
	float4 LIGHT;
	float4 LIGHT2;
	float4 LIGHT3;
}; 

#define PI 3.14159265359
#define TWO_PI (2.0*PI)

static const float4 BALL = float4(0.0,0.0,1.0,1.4);
static const float3 FLOOR_AMB = float3(0.65,1.4,0.5);
static const float3 CEIL_AMB = float3(0.95,0.4,0.40);

float mat(float4 ball, float3 pos, out int material)
{
    //try sphere first
   	float dist = length(ball.xyz - pos) - ball.w;
  
    material = 0;
    
    return dist;
}

float2 getUv(float3 normal)
{
    float2 xzNorm = normalize(normal.xz);
    return float2((acos(xzNorm.x) / TWO_PI), atan( normal.y/( length(normal.xz) ) ) / TWO_PI);
}

float3 tweakNormal(float2 uv, float3 normal, float freq, float blending)
{
    float s = sin(uv.x * freq);
    float c = cos(uv.y * freq);
    normal.x += blending*s;
    normal.z += blending*c;
    return normalize(normal);
}

void pointLight(float3 normal, float3 pos, float3 view, float3 lightPos, out float3 diffuse, out float3 spec, float specPow)
{
    float3 lightDir = normalize(lightPos - pos);
    diffuse = (saturate(dot(lightDir, normal))).xxx;
    
    float3 h = reflect(-view, normal);
    float nDoth = clamp(dot(-lightDir, h), 0.0, 1.0);    
    spec = (pow(saturate(nDoth.xxx), 10)).xxx;
    spec = lerp(spec, (pow(nDoth.xxx, 1.0)).xxx, 0.4);
}

void scene(float3 pos, float3 localPos, float3 view, float3 normal, float2 uv, out float3 col)
{
	float3 nLocalPos = normalize(localPos);
    float3 p = pos;
    float th = stripThickness_AA_unused.x/1000.0;
    float aa = stripThickness_AA_unused.y/1000.0;

    {
        float3 alb = float3(0.4, 0.08, 0.0);
        float3 stripsAlb =float3(0.01,0.01,0.01); 
		float isStrips = 1.0;
		isStrips *= smoothstep( 0.025-aa, 0.025+aa, abs(nLocalPos.x) );
		isStrips *= smoothstep( 0.025-aa, 0.025+aa, abs(nLocalPos.y) );
		isStrips *= smoothstep( th-aa, th+aa, abs(abs(nLocalPos.x) - 2.0*nLocalPos.y*nLocalPos.y - 0.15) );
		isStrips = 1.0 - isStrips;
        float3 occ = (1.0 - pow(saturate(-p.y), 4.0)).xxx;
        float nDotv = dot(normal, -view);
        float fresnel = clamp(0.3 + pow((1.0 - clamp(nDotv, 0.0, 1.0)), 5.0), 0.0, 1.0);
		float roughness = 2.0;
        float specForce = 1.0;
        normal = lerp(tweakNormal(uv, tweakNormal(uv, normal,3000.0,0.2), 100.0, 0.05), normal, pow(saturate(isStrips+(1.0 - saturate(nDotv))),0.5));
        float camSpec = 0.2*clamp(pow(saturate(nDotv).x, 1.0), 0.0, 0.3);
        nDotv = dot(normal, -view);
        alb = lerp(alb, stripsAlb, isStrips);
        
        roughness = lerp(roughness, 46.0, isStrips);
        specForce = lerp(specForce, 1.5, isStrips);

        float3 diffuse = 0.0f;
        float3 spec = 0.0f;
        pointLight(normal, p, view, LIGHT.xyz, diffuse, spec, roughness);
        float3 diffuse2 = 0.0f;
        float3 spec2 = 0.0f;
        pointLight(normal, p, view, LIGHT2.xyz, diffuse2, spec2, roughness);
        diffuse += diffuse2;
        spec += spec2;
        pointLight(normal, p, view, LIGHT3.xyz, diffuse2, spec2, roughness);
        diffuse += diffuse2;
        spec += spec2;
        float3 amb = lerp(FLOOR_AMB, CEIL_AMB, saturate(2.0*p.y));
        col =  alb*(occ*diffuse + amb) + occ*specForce*(spec+camSpec)*fresnel;//spec + alb * (clamp(dot(normal, lightDir),0.0,1.0) + occ*AMB) + occ*0.4*pow((1.0 - clamp(dot(normal, -ray), 0.0, 1.0)), 9.0);

    }
}

void vsMain(
	in float4 p  : POSITION0,
	in float4 localPos : POSITION1,
	in float3 n : NORMAL0,
	in float2 t : TEXTURE0,
	out float4 color : SV_Target)
{
	float3 view = normalize(p.xyz - worldCamera.xyz);
	float3 normal = normalize(n);

    float3 col = float3(0,0,0);
    scene(p.xyz, localPos.xyz, view, normal, t, col);
   
	color = float4(col, 1.0);
}

cbuffer uniformState
{
	float4x4 uWorld;
	float4x4 uProj;
};

struct VS_OUT
{
	float4 p : POSITION;
	float4 localPos : POSITION1;
	float3 normal : NORMAL0;
	float2 t : TEXTURE0;

};

VS_OUT  psMain(
	in float4 p0 : POSITION0,
	in float2 t0 : TEXCOORD0,
	in float3 n0 : NORMAL0,
	out float4 pos : SV_Position
)
{
	VS_OUT vo;
	float4x4 worldView = mul(uWorld,gView);
	vo.p =  mul(p0, worldView);
	vo.localPos = p0;
	vo.normal = mul(float4(n0,0.0),uWorld).xyz;
	vo.t = t0;
	pos = mul(vo.p,gProj);
	return vo;

}
