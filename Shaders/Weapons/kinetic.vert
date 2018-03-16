
#version 400 core

layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 norml;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;


out vec3 pos;

//degrees to radians
float dg2Rad(float degrees) {
    return (degrees * 3.1415927f / 180.0f);
}

