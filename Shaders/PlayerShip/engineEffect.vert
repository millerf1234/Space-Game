#version 400 core

layout(location = 0) in vec3 enginePos;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;
uniform float earlyThetaZ;

out vec3 pos;

//degrees to radians
float dg2Rad(float degrees) {
    return (degrees * 3.1415927f / 180.0f);
}


//------------------------------------------------------------------------------
//
//                      Aspect Ratio Matrices (only use 1)
//
//------------------------------------------------------------------------------

mat3 aspectRatioMatrix = mat3(1.0f,        0.0f     , 0.0f,
                              0.0f,  2650.0f/1600.0f, 0.0f,
                              0.0f,        0.0f     , 1.0f );

//This makes the ships a lot more flat (Not for gameplay use):
//mat3 aspectRatioMatrix = mat3(1.0f,       1.1f + 50.0f * sin(2.5f*time)     , 0.0f,
//                             -5.0f,       1.0f     , 0.0f,
//                              0.0f,       0.0f     , 1.0f );

//mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                              0.0f, 1920.0f/1080.0f, 0.0f,
//                              0.0f,       0.0f     , 1.0f );

//mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                              0.0f, 1670.0f/970.0f , 0.0f,
//                              0.0f,       0.0f     , 1.0f );

//The earlyRotationMatrixZ is for doing roll and allowing the model to move around at the same time
//(computing roll on a moving model would not be trival, but computing roll before doing other rotations/translations isn't that bad it turns out, can just use a basic rotation matrix to pre-set roll before everything else)

mat3 earlyRotationMatrixZ = mat3(    cos(earlyThetaZ)    ,     -sin(earlyThetaZ)    ,      0.0f          ,
                                     sin(earlyThetaZ)    ,      cos(earlyThetaZ)    ,      0.0f          ,
                                           0.0f          ,            0.0f          ,      1.0f          );




mat3 rotationMatrixX = mat3(               1.0f          ,            0.0f          ,       0.0f         ,
                            //0.0f  , cos(dg2Rad(90.0f)) , -sin(dg2Rad(90.0f)),  //was 18.0f
                            //0.0f  , sin(dg2Rad(90.0f)) ,  cos(dg2Rad(90.0f)));
                                           0.0f          ,        cos(thetaX)       ,   -sin(thetaX)     ,
                                           0.0f          ,        sin(thetaX)       ,    cos(thetaX)     );

mat3 rotationMatrixZ = mat3(           cos(thetaZ)       ,       -sin(thetaZ)       ,       0.0f         ,
                                       sin(thetaZ)       ,        cos(thetaZ)       ,       0.0f         ,
                                           0.0f          ,            0.0f          ,       1.0f         );

mat3 rotationMatrixY = mat3(           cos(thetaY)       ,            0.0f          ,   -sin(thetaY)     ,
                                           0.0f          ,            1.0f          ,       0.0f         ,
                                       sin(thetaY)       ,            0.0f          ,    cos(thetaY)     );

void main()
{
    pos = enginePos;
    //normal = norml;
    //normal = position; //I wrote this line after realizing normals don't work the way I thought/hoped they would work
    
    
    //Rotate, translate, aspect ratio?  or a different order?
    
    //Rotate:
    pos = earlyRotationMatrixZ * pos; //Do the rotation with the early rotation matrix first
    pos = rotationMatrixX * pos;
   // normal = rotationMatrixX * normal;
    pos = rotationMatrixZ * pos;
   // normal = rotationMatrixZ * normal;
    pos = rotationMatrixY * pos;
   // normal = rotationMatrixY * normal;
    
    //translate:
    pos.x += xTrans;
    pos.y += yTrans;
    pos.z += zTrans;
    
    //Aspect ratio
    pos = aspectRatioMatrix * pos;
    gl_Position = vec4(pos, zoom);
}





//#version 400 core
//
////Currently I am using the same vert shader for both the main and side engines
//
//in vec3 enginePos;
////in vec2 engineTxtr;
//
//uniform float time;
//uniform float zoom;
//uniform float xTrans;
//uniform float yTrans;
//uniform float zTrans;
//uniform float thetaX;
//uniform float thetaY;
//uniform float thetaZ;
//uniform float red; //These uniforms are unique to player:
//uniform float green;
//uniform float blue;
//
//out vec3 enginePosition;
////out vec2 engineTex;
//
//
////degrees to radians
//float dg2Rad(float degrees) {
//    return (degrees * 3.1415927f / 180.0f);
//}
//
////mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
////                              0.0f, 1920.0f/1080.0f, 0.0f,
////                              0.0f,       0.0f     , 1.0f );
//
////        mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
////                                      0.0f, 1670.0f/970.0f , 0.0f,
////                                      0.0f,       0.0f     , 1.0f );
//
//mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                              0.0f, 2650.0f/1600.0f, 0.0f,
//                              0.0f,       0.0f     , 1.0f );
//
//
////mat3 rotationMatrixX = mat3(    1.0f  ,      0.0f    ,     0.0f     ,
////                            0.0f  , cos(dg2Rad(90.0f)) , -sin(dg2Rad(90.0f)),
////                            0.0f  , sin(dg2Rad(90.0f)) ,  cos(dg2Rad(90.0f)));
//
//
//void main()
//{
//    enginePosition = enginePos;
//    // engineTex = engineTxtr;
//
//    //        mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//    //                                      0.0f, 2650.0f/1600.0f, 0.0f,
//    //                                      0.0f,       0.0f     , 1.0f );
//
//
//
//    enginePosition = enginePosition + vec3(0.0f, -1.75f /* * sin(time3D)*/, 0.0f);
//    //enginePosition = aspectRatioMatrix * rotationMatrixX * enginePosition;
//    enginePosition = aspectRatioMatrix * enginePosition;
//
//    //enginePosition.x += 0.005f*cos(time*100.0f);
//
//
//    gl_Position = vec4(enginePosition, 40.5 + zoom) ;// vec4(1.0f, 1.0f, 1.0f, 0.0f);
//
//}

