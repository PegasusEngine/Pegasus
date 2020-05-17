struct BaseSkyV2P
{
	float4 outPos      : SV_Position;
	float4 screenPosUvPos   : TEXCOORD0;
	float3 worldRayDir : TEXCOORD1;

};