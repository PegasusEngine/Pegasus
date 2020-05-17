//
//Cubefractal2 vs, by Kevin Boulanger 2013
//

#version 330 core
in vec2 p0;
out vec3 p,d;
layout(std140) uniform uniformState
{
float screenRatio;
float time;
};
void main(){
    gl_Position=vec4(p0.x,p0.y*screenRatio,0.0,1.0);
    p = vec3(0,0,time*0.15);
    d = vec3(p0.x * cos(time*0.25) + sin(time*0.25), p0.y + .4*sin(time*0.25), cos(time*0.25) - p0.x * sin(time*0.25));
}
