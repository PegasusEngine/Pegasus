// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html

#ifndef SAMPLING_H
#define SAMPLING_H

#include <RenderSystems/Math.h>


float radicalInverse_VdC(uint bits)
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 hammersley2d(uint i, uint N)
{
     return float2(float(i)/float(N), radicalInverse_VdC(i));
}

float3 hemisphereSample_uniform(float2 uv)
{
     float phi = uv.y * 2.0 * C_PI;
     float cosTheta = 1.0 - uv.x;
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
    
float3 hemisphereSample_cos(float2 uv)
{
     float phi = uv.y * 2.0 * C_PI;
     float cosTheta = sqrt(1.0 - uv.x);
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float3 sphereSample_uniform(float2 uv)
{
	 float phi = uv.y * 2.0 * C_PI;
     float cosTheta = 1.0 - 2.0*uv.x;
     float sinTheta = max(0.0,sqrt(1.0 - cosTheta * cosTheta));
     return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

#endif