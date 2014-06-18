//
// Cube vs by Kleber Garcia 2014
//

#version 330 core

in vec4 p0;
in vec3 n0;
out vec4 p;
out vec3 normal;

uniform float uTime;

void main()
{
	float hRot = 2.2 * uTime;
	vec2 sincosHRot = vec2(cos(hRot),sin(hRot));
	p = vec4( dot(p0.xz,sincosHRot.xy), p0.y, dot(p0.xz,vec2(-sincosHRot.y,sincosHRot.x)), p0.w );

	float vRot = 6.226 * uTime;
	vec2 sincosVRot = vec2(cos(vRot),sin(vRot));
	p.xyz = vec3( p.x, dot(p.yz,sincosVRot.xy), dot(p.yz,vec2(-sincosVRot.y,sincosVRot.x)) );

	p *= 0.4;
    
    //aspect correctness
    p.y *= 900.0 / 720.0;

	normal = n0;
    gl_Position = vec4(p.xyz, 1.0);
}

