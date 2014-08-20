//
//Psybeads vs, by Kevin Boulanger 2013
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
    p=3.0*vec3(cos(time),sin(time),time);
    d=vec3(p0.x*p.x-p0.y*p.y,p0.y*p.x+p0.x*p.y,1);
}
