//Both this shader and mainEngineEffect.frag are intended to be used with engineEffect.vert

#version 410 core

in vec3 pos;


uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;

uniform float redinate; //a factor of how red effect should be.
uniform float blueify; //a factor of how blue the effect should be


out vec4 color;

void main() {
    
    color = vec4(redinate / 0.85f, 0.75f * abs(cos(-(pos.x * pos.x - pos.y * pos.y + time*20.0f))), blueify * 1.1f, 1.0f);
    
    
}
//void main() {
    //For more indepth info on gl_FragCoord use
    //See: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml
    
    
    
    
    //color = vec4(enginePosition, 1.0f);// + vec4 (engineTex, engineTex);
    
     //color = vec4(0.8f + 0.25f*(gl_FragCoord.x * gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.y) + cos(-time*5.0f), 0.95f * cos(-(pos.x * pos.x - pos.y * pos.y + time*10.0f)), 0.2f, 1.0f);
    
    //color = vec4(0.8f, 0.35f * abs(cos(-(pos.x * pos.y * pos.x - pos.y * pos.y + time * 10.0f*sin(time))) + tan(max(thetaX + thetaY + thetaZ, 0.01f))), blueify * 0.66f, 1.0f);
    
    
    
    
    //Engine position is stagnant, fragCoord moves with the model
    //color = vec4(0.8f + 0.75f*(enginePosition.x * enginePosition.x + enginePosition.y * enginePosition.y) + cos(time*5.0f), 0.75, 0.2f, 1.0f);
//}


