#ifndef CAM_COMMON_H
#define CAM_COMMON_H

cbuffer __camera_cbuffer
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gViewProj;
    float4x4 gInvViewProj;
	float4   gEyePos;
	float4   gEyeDir;
};

#endif
