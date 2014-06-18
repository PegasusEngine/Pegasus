//
// Cube vs by Kleber Garcia 2014
//

#version 330 core

in vec4 p0;
in vec3 n0;
out vec4 p;
out vec3 normal;

layout(std140) uniform uniformState
{
    float uTime;
    float uAspect;
};

void main()
{
	float hRot = 2.2 * uTime;
	vec2 sincosHRot = vec2(cos(hRot),sin(hRot));
    vec4 pos = p0;
	pos.xyz = vec3( dot(pos.xz,sincosHRot.xy), pos.y, dot(pos.xz,vec2(-sincosHRot.y,sincosHRot.x)) );

	float vRot = 6.226 * uTime;
	vec2 sincosVRot = vec2(cos(vRot),sin(vRot));
	pos.xyz = vec3( pos.x, dot(pos.yz,sincosVRot.xy), dot(pos.yz,vec2(-sincosVRot.y,sincosVRot.x)) );

	pos *= 0.4;
    
    //aspect correctness
    pos.y *= uAspect;

	normal = n0;
    gl_Position = vec4(pos.xyz, 1.0);

    p = pos;
}

