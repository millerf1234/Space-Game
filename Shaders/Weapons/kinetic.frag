//Good reference for future: http://www.shaderific.com/glsl-functions/
//Want to make kinetic projectiles be orangish/redish, maybe with a flaming glow?

#version 400 core

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
    color = vec4(0.9f, 0.45f, 0.75f, 1.0f);
    //Once I get Kinetic working, use an effect like this:
    //vec4 glow_color = vec4(1.0f, 0.8f, 1.0f, 1.0f) - exp(vec4(0.5f*time, 0.45*time, 0.0f, 0.0f));
    //color = clamp(glow_color, vec4(0.35f, 0.15f, 0.0f, 1.0f), glow_color); //Clamp makes it so color doesn't disappear completely
}
