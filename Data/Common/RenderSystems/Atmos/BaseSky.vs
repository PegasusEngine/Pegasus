#include "RenderSystems/Camera/Common.h"
#include "RenderSystems/Atmos/BaseCommon.h"

void main(in float4 p0 : POSITION0, out BaseSkyV2P v2p)
{
	float4 worldRayPos = mul(float4(p0.x,p0.y,0.0,1.0),gInvViewProj);
	v2p.worldRayDir = worldRayPos.xyz/worldRayPos.w - gEyePos.xyz;
	
	float2 uvCoords = p0.xy*float2(0.5,-0.5) + 0.5;
	v2p.screenPosUvPos = float4(p0.x,p0.y,uvCoords.x,uvCoords.y);
	v2p.outPos = float4(p0.x,p0.y,1.0,1.0);
}
