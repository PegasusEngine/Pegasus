//
//Cubefractal vs, by Kevin Boulanger 2013
//

#version 330 core

in vec2 p0;
out vec3 p,d;
uniform float time;
uniform float screenRatio;
void main(){
    gl_Position=vec4(p0.x,p0.y*screenRatio,0.0,1.0);
    p = vec3(0,0,time);
    d = vec3(p0.x * cos(time) + sin(time), p0.y, cos(time) - p0.x * sin(time));
}
