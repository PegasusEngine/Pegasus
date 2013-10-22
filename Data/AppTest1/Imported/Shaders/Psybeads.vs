//
//Psybeads vs, by Kevin Boulanger 2013
//

#version 330 core

layout(location = 0) in vec2 vPos;

out vec3 p,d;
uniform float time;
uniform float screenRatio;
void main(){
    gl_Position=vec4(vPos.x,vPos.y*screenRatio,0.0,1.0);
    p=3.0*vec3(cos(time),sin(time),time);
    d=vec3(vPos.x*p.x-vPos.y*p.y,vPos.y*p.x+vPos.x*p.y,1);
}
