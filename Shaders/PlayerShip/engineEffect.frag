#version 400 core

in vec3 enginePosition;
//in vec2 engineTex;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;
uniform float red; //These uniforms are unique to player:
uniform float green;
uniform float blue;

out vec4 color;

void main() {
    
    //color = vec4(enginePosition, 1.0f);// + vec4 (engineTex, engineTex);
    
    //Switch the fragCoords to enginePosition
    color = vec4(0.8f + 0.25f*(gl_FragCoord.x * gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.y) + cos(-time*5.0f), 0.95f * cos(-(enginePosition.x * enginePosition.x - enginePosition.y * enginePosition.y + time*10.0f)), 0.2f, 1.0f);
    
    
    //Engine position is stagnant, fragCoord moves with the model
    //color = vec4(0.8f + 0.75f*(enginePosition.x * enginePosition.x + enginePosition.y * enginePosition.y) + cos(time*5.0f), 0.75, 0.2f, 1.0f);
}

