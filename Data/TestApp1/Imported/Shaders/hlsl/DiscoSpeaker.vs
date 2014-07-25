//
// DiscoSpeaker vs, by Kleber Garcia 2014
//

void main(
	in float4 p0 : POSITION0,
	out float4 pos : SV_POSITION,
	out float4 p : POSITION0
)
{
    pos=float4(p0.x,p0.y,0.0,1.0);
	p = pos;
}



