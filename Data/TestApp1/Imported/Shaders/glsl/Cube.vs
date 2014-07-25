//
// Cube vs by Kleber Garcia 2014
//

#version 330 core

in vec4 p0;
in vec2 t0;
in vec3 n0;
out vec4 p;
out vec2 t;

out vec3 normal;

layout(std140, row_major) uniform uniformState
{
    // place here all the uniforms to be used. Be sure to manually pad to a multiple of 16 bytes.
    mat4x4 uTransform;
};

void main()
{
    // IMPORTANT NOTE!!!
    // OpenGL is column major by default. Our math library is row major.
    // To do transformations correctly we either do:
    //  p0 * uTransform
    // or use a layout(row_major) query in the uniform (or buffer) we are using.
    // in the case of this uniform block, we actually use the row_major query
    vec4 pos = uTransform * p0;
    t = t0;

	pos *= 0.2;

	normal = n0;
    gl_Position = vec4(pos.xyz, 1.0);

    p = pos;
}


