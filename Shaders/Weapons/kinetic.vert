#version 410 core

layout(location = 0) in vec3 position;

uniform float time;
uniform float zoom;
uniform float earlyThetaZ;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;
uniform float red;
uniform float green;
uniform float blue;

out vec3 pos;

//degrees to radians
float dg2Rad(float degrees) {
    return (degrees * 3.1415927f / 180.0f);
}

mat3 aspectRatioMatrix = mat3(  1.0f,         0.0f      ,    0.0f,
                                0.0f,   2650.0f/1600.0f ,    0.0f,
                                0.0f,         0.0f      ,    1.0f );


mat3 earlyRotationMatrixZ = mat3(cos(earlyThetaZ)    ,     -sin(earlyThetaZ)    ,      0.0f          ,
                                 sin(earlyThetaZ)    ,      cos(earlyThetaZ)    ,      0.0f          ,
                                       0.0f          ,             0.0f         ,      1.0f          );

mat3 rotationMatrixX      = mat3(       1.0f         ,             0.0f         ,       0.0f         ,
                                        0.0f         ,          cos(thetaX)     ,   -sin(thetaX)     ,
                                        0.0f         ,          sin(thetaX)     ,    cos(thetaX)     );

mat3 rotationMatrixZ      = mat3(   cos(thetaZ)      ,         -sin(thetaZ)     ,       0.0f         ,
                                    sin(thetaZ)      ,          cos(thetaZ)     ,       0.0f         ,
                                        0.0f         ,             0.0f         ,       1.0f         );

mat3 rotationMatrixY      = mat3(   cos(thetaY)      ,             0.0f         ,   -sin(thetaY)     ,
                                        0.0f         ,             1.0f         ,       0.0f         ,
                                    sin(thetaY)      ,             0.0f         ,    cos(thetaY)     );


void main()
{
    pos = position;
    
    //Rotate:
    pos = earlyRotationMatrixZ * pos;
    pos = rotationMatrixX * pos;
    pos = rotationMatrixZ * pos;
   // pos = rotationMatrixY * pos;
    
    //Translate:
    pos.x += xTrans;
    pos.y += yTrans;
    pos.z += zTrans;
    
    //Aspect Ratio
    pos = aspectRatioMatrix * pos;
    gl_Position = vec4(pos, zoom);
}



