//Good reference for future: http://www.shaderific.com/glsl-functions/
//Want to make kinetic projectiles be orangish/redish, maybe with a flaming glow?

#version 410 core

in vec3 pos;
out vec4 color;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;



void main() {
   
    //Here's a good one (and the one I am going to go with
   // color = vec4(1.65f*cos(1.65f * time), 0.35f*sin(1.15f*time), 0.15f + 0.1f*sin(time), 1.0f );
    
    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    
    //color = vec4(0.8f * tan(5.0f * time) - cos(pos.x * (gl_FragCoord.x - tan(gl_FragCoord.y))) + 0.25f * cos(10.0f * time), 0.5f + sin(time * pos.x * pos.y - pos.x), 0.75 * cos(-sin(0.45f * time)), 1.0f );
}
