//This frag shader should use the same vert shader as playerParticle.frag (i.e. it should use playerParticle.vert)


#version 410 core

uniform float time;

out vec4 color;


void main() {
    
    float red1 = sqrt(pow(sin(0.5*gl_FragCoord.x), 2.0) + pow(cos(0.5*gl_FragCoord.y), 2.0));
    float green1 = sqrt(0.75*pow(sin(0.1*gl_FragCoord.x + 0.5), 2.0) + 0.75*pow(cos(0.1*gl_FragCoord.y + 0.5), 2.0));
    
    red1 = max(0.5, red1 * abs(sin(time)));
    green1 = min(red1, 0.75 * green1);
    
    color = vec4(red1, green1, 0.1f, 1.0f);
    //color = vec4(red + 0.15 * sin(time), green, blue, 1.0f);
    
    
}
