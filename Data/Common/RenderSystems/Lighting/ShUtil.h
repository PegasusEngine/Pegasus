#include "RenderSystems/Math.h"
#ifndef SH_UTIL
#define SH_UTIL

//Shader sh utility file

struct ShL1
{
	float4 v[4];
};

struct ShL2
{
	float4 v[9];
};

#define L0_M0(_n)  (1.0/C_SQRT_PI)
#define L1_M_1(_n) ((sqrt(3.0)*_n.y)/(2.0*C_SQRT_PI))
#define L1_M0(_n)  ( (sqrt(3.0)*_n.z)/(2.0*C_SQRT_PI))
#define L1_M1(_n)  ( (sqrt(3.0)*_n.x)/(2.0*C_SQRT_PI))
#define L2_M_2(_n) (-(sqrt(15.0)*_n.y*_n.x)/(2.0*C_SQRT_PI))
#define L2_M_1(_n) ( (sqrt(15.0)*_n.y*_n.z)/(2.0*C_SQRT_PI))
#define L2_M0(_n)  ((sqrt(5.0)*(3.0*_n.z*_n.z-1.0))/(4.0*C_SQRT_PI))
#define L2_M1(_n)  (-(sqrt(15.0)*_n.x*_n.z)/(2.0*C_SQRT_PI))
#define L2_M2(_n)  ((sqrt(15.0)*(_n.x*_n.x-_n.y*_n.y))/(4.0*C_SQRT_PI))

ShL1 L1Basis(float3 n)
{
	ShL1 l1;
	l1.v[0] = L0_M0(n);
	l1.v[1] = L1_M_1(n);
	l1.v[2] = L1_M0(n);
	l1.v[3] = L1_M1(n);
	return l1;
}

ShL2 L2Basis(float3 n)
{
	ShL2 l2;
	l2.v[0] = L0_M0(n);
	l2.v[1] = L1_M_1(n);
	l2.v[2] = L1_M0(n);
	l2.v[3] = L1_M1(n);
	l2.v[4] = L2_M_2(n);
	l2.v[5] = L2_M_1(n);
	l2.v[6] = L2_M0(n);
	l2.v[7] = L2_M1(n);
	l2.v[8] = L2_M2(n);
	return l2;
}

void projectSh(in ShL1 a, in ShL1 b, out ShL1 r)
{
	[unroll]
	for (uint i = 0; i < 4; ++i)
		r.v[i] = a.v[i]*b.v[i];
}

void projectSh(in ShL2 a, in ShL2 b, out ShL2 r)
{
	[unroll]
	for (uint i = 0; i < 9; ++i)
		r.v[i] = a.v[i]*b.v[i];	
}

void shAdd(inout ShL1 v, in ShL1 a)
{
	[unroll]
	for (uint i = 0; i < 4; ++i)
		v.v[i] += a.v[i];
}

void shAdd(inout ShL2 v, in ShL2 a)
{
	[unroll]
	for (uint i = 0; i < 9; ++i)
		v.v[i] += a.v[i];
}

void shMultiply(inout ShL1 v, in float4 value)
{
	[unroll]
	for (uint i = 0; i < 4; ++i)
		v.v[i] *= value;
}

void shMultiply(inout ShL2 v, in float4 value)
{
	[unroll]
	for (uint i = 0; i < 9; ++i)
		v.v[i] *= value;
}


float3 evaluateSh(in ShL2 sh, float3 n)
{
	float c1 = 0.429043f;
	float c2 = 0.511664f;
	float c3 = 0.743125f;
	float c4 = 0.886227f;
	float c5 = 0.247708f;
	// l = 0, m = 0
	float3 L00 = sh.v[0].xyz;
 
	// l = 1, m= -1 0 +1 
	float3 L1_1 = sh.v[1].xyz;
	float3 L10 = sh.v[2].xyz;
	float3 L11 = sh.v[3].xyz;

	// l = 2, m= -2 -1 0 +1 +2
	float3 L2_2 = sh.v[4].xyz;
	float3 L2_1 = sh.v[5].xyz;
	float3 L20 = sh.v[6].xyz;
	float3 L21 = sh.v[7].xyz;
	float3 L22 = sh.v[8].xyz;

	float x = n.x;
	float y = n.y;
	float z = n.z;

	float x2 = n.x * n.x;
	float y2 = n.y * n.y; 
	float z2 = n.z * n.z;

	return (c1*(x2 - y2)) * L22 
		+ (c3*z2)*L20 
		+ c4*L00 
		- c5*L20
		+ (2.0f*c1) * ((x*y)*L2_2 + (x*z)*L21 + (y*z)*L2_1)
		+ (2.0f*c2) * (L11*x + L1_1*y + L10*z);
}

float3 evaluateSh(in ShL1 sh, float3 n)
{
	float c2 = 0.511664f;
	float c4 = 0.886227f;
	return (c4*sh.v[0]+ 2.0*c2*(n.x * sh.v[0] + n.y * sh.v[1] + n.z * sh.v[2])).rgb;
}

#endif