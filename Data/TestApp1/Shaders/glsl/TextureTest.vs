//
// TextureTest vs, by Kevin Boulanger 2014
//

#version 330 core

layout(location = 0) in vec2 p0;

layout(std140) uniform uniformState
{
float screenRatio;
};

out vec2 texCoords;

void main()
{
    gl_Position = vec4(p0.x * 0.9 / screenRatio, p0.y * 0.9, 0.0, 1.0);
	texCoords = p0.xy * 0.5 + 0.5;
}
