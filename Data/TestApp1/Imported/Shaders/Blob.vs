//
//Blob Triangle vs, by Kleber 2013
//


#version 430 core 

layout(location = 0) in vec3 vPos;
out vec4 screenPos; 
uniform float time;        
void main()           
{
    vec4 aspect = vec4(960.0f/540.0f, 1,1,1);
    gl_Position = aspect*vec4(vPos + vec3(0.2*sin(time*0.001),0.4*cos(time*0.001), 0) , 1.0f);
    float st = sin(time*0.005);
    float ct = cos(time*0.005);
    vec4 r = vec4(st,ct,-ct,st);
    gl_Position.xy = vec2(dot(gl_Position.xy,r.xy),dot(gl_Position.xy,r.zw));
    screenPos = gl_Position ;
}

