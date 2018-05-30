
#version 410 core

//in vec4 pos;

uniform float time;
uniform float red;
uniform float green;
uniform float blue;

out vec4 color;


void main() {
    
    float red = sqrt(pow(sin(0.5*gl_FragCoord.x), 2.0) + pow(cos(0.5*gl_FragCoord.y), 2.0));
    float green = sqrt(0.75*pow(sin(1.0*gl_FragCoord.x + 0.5), 2.0) + 0.75*pow(cos(1.0*gl_FragCoord.y + 0.5), 2.0));
    color = vec4(red, green, 0.1f, 1.0f);
    //color = vec4(red + 0.15 * sin(time), green, blue, 1.0f);
    
    
}






//
//#version 410 core
//
//in vec3 pos;
//uniform float time;
//uniform float zoom;
//uniform float red;
//uniform float green;
//uniform float blue;
//out vec4 color;
//
//
//void main() {
//
//    color = vec4(red, green, blue, 1.0f);
//
//
//}

