//This frag shader should use the same vert shader as playerParticle.frag (i.e. it should use playerParticle.vert)


#version 410 core

uniform float time;

out vec4 color;


void main() {
    
    float red = sqrt(pow(sin(0.5*gl_FragCoord.x), 2.0) + pow(cos(0.5*gl_FragCoord.y), 2.0));
    float green = sqrt(0.75*pow(sin(0.1*gl_FragCoord.x + 0.5), 2.0) + 0.75*pow(cos(0.1*gl_FragCoord.y + 0.5), 2.0));
    
    red = max(0.65, red * abs(sin(time)));
    green = min(red, 0.75 * green);
    
    
    color = vec4(red + 1.0/(pow((gl_FragCoord.x / 20.0f), 2.0f) + pow((gl_FragCoord.y / 20.0f), 2.0f)), green, 0.1f, 1.0f);
    
    //Alternative Color    Starts Blue/Purlpe and progresses to red/orange
    //color = vec4(red + 1.0/(pow((gl_FragCoord.x / 20.0f), 2.0f) + pow((gl_FragCoord.y / 20.0f), 2.0f)), green, 0.1f + 1.0f / 100.0f * floor(abs(75.0f * cos(0.3f * time))), 1.0f);
    
    
}













////This frag shader should use the same vert shader as playerParticle.frag (i.e. it should use playerParticle.vert)
//
//
//#version 410 core
//
//uniform float time;
//
//out vec4 color;
//
//
//void main() {
//
//    float red = sqrt(pow(sin(0.5*gl_FragCoord.x), 2.0) + pow(cos(0.5*gl_FragCoord.y), 2.0));
//    float green = sqrt(0.75*pow(sin(0.1*gl_FragCoord.x + 0.5), 2.0) + 0.75*pow(cos(0.1*gl_FragCoord.y + 0.5), 2.0));
//
//    red = max(0.6, red * abs(sin(time)));
//    green = min(red, 0.75 * green);
//
//    color = vec4(red + 1.0/(pow((gl_FragCoord.x / 20.0f), 2.0f) + pow((gl_FragCoord.y / 20.0f), 2.0f)), green, 0.1f, 1.0f);
//    //color = vec4(red + 0.15 * sin(time), green, blue, 1.0f);
//
//}
