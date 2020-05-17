//
// DiscoSpeaker vs, by Kleber Garcia 2014
//

#version 330 core

in vec4 p0;
out vec4 p;

void main(){
    gl_Position=vec4(p0.x,p0.y,0.0,1.0);
    p = p0;
}



