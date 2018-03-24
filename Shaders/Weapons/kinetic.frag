//Good reference for future: http://www.shaderific.com/glsl-functions/
//Want to make kinetic projectiles be orangish/redish, maybe with a flaming glow?

#version 410 core

/*layout(location = 0)*/ in vec3 pos;
/*layout(location = 0)*/ out vec4 color;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;
//uniform float red;  //Make the bullet color match the player color?
//uniform float green;
//uniform float blue;


void main() {
    //Until I get kinetic working, make it easy to see by giving it a good color
    //color = vec4(0.9f, 0.45f, 0.75f + 0.25*sin(time), 1.0f);
    
    //Once I get Kinetic working, use an effect like this:
    //vec4 glow_color = vec4(1.0f, 0.8f, 0.5f, 1.0f) - exp(vec4(time, time, 0.0f, 0.0f));
    //color = clamp(glow_color, vec4(0.65f, 0.65f, 0.0f, 1.0f), glow_color); //Clamp makes it so color doesn't disappear completely
    
    //float futureTime = time + 3.141592f;
    //Try again:
    //color = vec4(1.1f * cos(0.5f*time), 0.6f * cos(2.0f * time - 3.14159f / 0.5f), 0.15f, 1.0f);
    //color = vec4(abs(1.1f * cos(futureTime)), 0.6f * cos(2.0f * futureTime - 3.14159f / 0.5f), 0.15f, 1.0f);
    
    //Here's a good one
    color = vec4(cos(0.9f * time), 0.4f*sin(1.15f*time), 0.15f + 0.1f*sin(time), 1.0f );
    
    //color = vec4(cos(0.9f * time - pos.x + pos.y*tan(pos.x-yTrans)), 0.6f*sin(1.15f*time + pow(pos.x, pos.y-pos.x)), 0.15f + 0.1f*sin(time) + cos(10.0f *pos.x + 10.0f * pos.y), 1.0f );
}
