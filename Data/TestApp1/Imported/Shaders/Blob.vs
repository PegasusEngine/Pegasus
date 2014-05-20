//
//Blob Triangle vs, by Kleber 2013
//


#version 430 core 

in vec4 p0;
in vec2 t0;
out vec4 screenPos; 
uniform float time;        
void main()           
{
    vec4 aspect = vec4(960.0f/540.0f, 1,1,1);
    float st = sin(time*0.005);
    float ct = cos(time*0.005);
    vec4 r = vec4(st,ct,-ct,st);
    gl_Position.xy = p0.xyzw;
    screenPos = t0.xyxy ;
}



