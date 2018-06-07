//Both this shader and sideEngineEffect.frag are intended to be used with engineEffect.vert

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
    //Before I incorperated redinate and blueify, the frag shader was just one line:
    //color = vec4(1.0f, 0.95f * cos(-(pos.x * pos.x - pos.y * pos.y + time*10.0f)), 0.2f, 1.0f);
    
    //Now that I have included redinate and blueify, the shader is:
    //color = vec4(redinate / 0.85f, 0.95f * cos(-(pos.x * pos.x - pos.y * pos.y + time*10.0f)), blueify, 1.0f);
    
    
    //Everything below here is experimental
    color = vec4(redinate / 0.85f,
                 abs(tan(redinate/blueify) +
                     (pow(gl_FragCoord.x / (gl_FragCoord.y * time), 2.0f)) /
                     (pow(gl_FragCoord.y / (gl_FragCoord.x * time), 2.0f)) ) *
                                        floor(1000.0f*cos(-(pos.x * pos.x - pos.y * pos.y + time*(25.0f))) / 1000.0f),
                 blueify,
                 1.0f);
    //color = color.normalize();
}



////void main() {
//    //For more indepth info on gl_FragCoord use
//    //See: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml
//
//
//    //color = vec4(enginePosition, 1.0f);// + vec4 (engineTex, engineTex);
//
//    //color = vec4(0.8f + 0.25f*(gl_FragCoord.x * gl_FragCoord.x + gl_FragCoord.y * gl_FragCoord.y) + cos(-time*5.0f), 0.95f * cos(-(pos.x * pos.x - pos.y * pos.y + time*10.0f)), 0.2f, 1.0f);
//
////    color = vec4(1.0f, 0.95f * cos(-(pos.x * pos.x - pos.y * pos.y + time*10.0f)), 0.2f, 1.0f);
//
//    //Engine position is stagnant, fragCoord moves with the model
//    //color = vec4(0.8f + 0.75f*(enginePosition.x * enginePosition.x + enginePosition.y * enginePosition.y) + cos(time*5.0f), 0.75, 0.2f, 1.0f);
//}

