//*******  BRDF Functions                  ************//
// Uber shader for lighting components                //
//****************************************************//
#ifndef _BRDF_
#define _BRDF_

struct BrdfInfo
{
	float reflectance;
	float roughness;
	float NdV;
	float NdH;
	float VdH; 
	float LdH;
	float NdL;
};

#define PI 3.14159

void GetBrdfInfo(out BrdfInfo info, float3 V, float3 L, float3 N, float smoothness, float reflectance)
{
	float3 H = normalize(L+V);
	info.roughness = 1.0 - smoothness;
	info.reflectance = reflectance;
	info.NdV = abs(dot(N,V))+0.0000001;
	info.NdH = saturate(dot(N,H));
	info.VdH = saturate(dot(V,H));
	info.LdH = saturate(dot(L,H));
	info.NdL = saturate(dot(N,L));
}


// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float roughness, float NdH )
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = ( NdH * a2 - NdH ) * NdH + 1;	// 2 mad
	return a2 / ( PI*d*d );			// 4 mul, 1 rcp
} 

float V_Smith( float roughness, float NoV, float NoL )
{
	float a = roughness * roughness;
	float a2 = a*a;

	float Vis_SmithV = NoV + sqrt( NoV * (NoV - NoV * a2) + a2 );
	float Vis_SmithL = NoL + sqrt( NoL * (NoL - NoL * a2) + a2 );
	return rcp( Vis_SmithV * Vis_SmithL ); 
}

float3 F_Schlick( float3 fresnelCol, float LdH )
{ 
	float3 f90 = saturate(50*dot(fresnelCol,0.33)).xxx;
	float3 f0 = fresnelCol;
	float Fc = pow( 1 - LdH,5.0);	 
	return f0+( f90 - f0 ) *Fc;
	
} 

float3 ComputeSpecular(float3 lightIntensity, in BrdfInfo brdfInfo)
{ 
	float D = D_GGX(brdfInfo.roughness,brdfInfo.NdH);
	float3 F = F_Schlick(brdfInfo.reflectance.xxx,brdfInfo.LdH);
	float V = V_Smith(brdfInfo.roughness,brdfInfo.NdV,brdfInfo.NdL);
	return lightIntensity*D*F*V;
}

#endif