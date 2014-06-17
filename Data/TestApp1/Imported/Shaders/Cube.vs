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
	p0.xyz = vec3( dot(p0.xz,sincosHRot.xy), p0.y, dot(p0.xz,vec2(-sincosHRot.y,sincosHRot.x)) );

	float vRot = 6.226 * uTime;
	vec2 sincosVRot = vec2(cos(vRot),sin(vRot));
	p0.xyz = vec3( p0.x, dot(p0.yz,sincosVRot.xy), dot(p0.yz,vec2(-sincosVRot.y,sincosVRot.x)) );

	p0 *= 0.4;
    
    //aspect correctness
    p0.y *= 900.0f / 720.0f;

	normal = n0;
    gl_Position.xyz = vec4(p0.xyz, 1.0);

    p = p0;
}

