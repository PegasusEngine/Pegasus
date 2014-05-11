//
// TextureTest vs, by Kevin Boulanger 2014
//

#version 330 core

layout(location = 0) in vec2 vPos;
uniform float screenRatio;
out vec2 texCoords;

void main()
{
    gl_Position = vec4(vPos.x * 0.9 / screenRatio, vPos.y * 0.9, 0.0, 1.0);
	texCoords = vPos.xy * 0.5 + 0.5;
}
