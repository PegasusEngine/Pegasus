#include "RenderSystems/Math.h"
#ifndef SH_UTIL
#define SH_UTIL

//Shader sh utility file

struct ShL1
{
	float4 v;
};

struct ShL2
{
	float4 v1;
	float4 v2;
	float2 v3;
};

#define L0_M0 (_n) (1.0/C_SQRT_PI)
#define L1_M_1(_n) (-(sqrt(3.0)*_n.y)/(2.0*C_SQRT_PI))
#define L1_M0 (_n) ( (sqrt(3.0)*_n.z)/(2.0*C_SQRT_PI))
#define L1_M1 (_n) ( (sqrt(3.0)*_n.x)/(2.0*C_SQRT_PI))
#define L2_M_2(_n) ( (sqrt(15.0)*_n.y*_n.x)/(2.0*C_SQRT_PI))
#define L2_M_1(_n) (-(sqrt(15.0)*_n.y*_n.z)/(2.0*C_SQRT_PI))
#define L2_M0 (_n) ((sqrt(5.0)*(3.0*_n.z*_n.z-1.0))/(4.0*C_SQRT_PI))
#define L2_M1 (_n) (-(sqrt(15.0)*_n.x*_n.z)/(2.0*C_SQRT_PI))
#define L2_M2 (_n) ((sqrt(15.0)*(_n.x*_n.x-_n.y*_n.y))/(4.0*C_SQRT_PI))

ShL1 L1Basis(float3 n)
{
	ShL1 1l;
	1l.v = float4(
		L0_M0(n),
		L1_M_1(n),
		L1_M0(n),
		L1_M1(n)
	);
	return l1;
}

ShL2 L2Basis(float3 n)
{
	ShL2 l2;
	l2.v1 = float4(L0_M0 (n),L1_M_1(n),L1_M0(n),L1_M1(n));
	l2.v2 = float4(L2_M_2(n),L2_M_1(n),L2_M0(n),L2_M1(n));
	l2.v3 = float2(L2_M2(n),0);
	return l2;
}

#endif