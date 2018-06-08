#version 400 core

/*layout(location = 0)*/ in vec2 position;
layout(location = 1) in vec3 colr;
layout(location = 2) in vec2 texCoord;

//uniform float time;
//uniform float tex;
//uniform float zoom;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;

uniform float aspectRatio;


out vec2 pos;
out vec3 COLR;
out vec2 TexCoor;

vec3 BACKGROUND_OFFSET = vec3(0.0f, 0.0f, 1.0f); //Place Background in the furthest back plane before culling

void main()
{
    pos = position;
    COLR = colr;
    TexCoor = texCoord;
    
    
    vec3 temp = vec3(position, 0.0f);
    temp.x += xTrans;
    temp.y += yTrans;
    temp.z += zTrans;
    
    
    mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
                                  0.0f, aspectRatio /* 3840.0f/2160.0f */, 0.0f,       //1500.0f/1080.0f  is good for moon,  1700.0f/1080.0f is good for jupiter
                                  0.0f,       0.0f     , 1.0f );
    
//    mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                                  0.0f, 1800.0f/1080.0f /* 3840.0f/2160.0f */, 0.0f,       //1500.0f/1080.0f  is good for moon,  1700.0f/1080.0f is good for jupiter
//                                  0.0f,       0.0f     , 1.0f );
    
    //pos = vec3( aspectRatioMatrix * vec3(position.x, position.y, 0.1f));
    
    temp = vec3( aspectRatioMatrix * vec3(position.x + 0.5f + xTrans, position.y + yTrans, 0.0f) + BACKGROUND_OFFSET);
    
    
    
    //gl_Position = vec4(temp /*+ vec3(xCoor, yCoor, 0.0f)*/, 1.0 + zoom);
    
    //gl_Position = vec4(pos, -1.5);
   // gl_Position = vec4(position.x, position.y, 0.0, 2.0f);
    gl_Position = vec4(temp, zoom);
    
    //gl_PointSize = 10 + sin(time);
}

