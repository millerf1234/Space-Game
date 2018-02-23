


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
out vec3 normal;

//degrees to radians
float dg2Rad(float degrees) {
    return (degrees * 3.1415927f / 180.0f);
}


//mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                              0.0f, 1920.0f/1080.0f, 0.0f,
//                              0.0f,       0.0f     , 1.0f );

//        mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                                      0.0f, 1670.0f/970.0f , 0.0f,
//                                      0.0f,       0.0f     , 1.0f );

mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
                              0.0f, 2650.0f/1600.0f, 0.0f,
                              0.0f,       0.0f     , 1.0f );

mat3 rotationMatrixX = mat3(    1.0f  ,       0.0f         ,       0.0f         ,
                                0.0f  , cos(dg2Rad(90.0f)) , -sin(dg2Rad(90.0f)),  //was 18.0f
                                0.0f  , sin(dg2Rad(90.0f)) ,  cos(dg2Rad(90.0f)));

mat3 rotationMatrixZ = mat3(    cos(thetaZ), -sin(thetaZ),  0.0f,
                                sin(thetaZ),  cos(thetaZ),  0.0f,
                                    0.0f    ,     0.0f   ,  1.0f);

mat3 rotationMatrixY = mat3(cos(thetaY), 0.0f, -sin(thetaY),
                                0.0f   , 1.0f,      0.0f   ,
                            sin(thetaY), 0.0f,  cos(thetaY));


void main()
{
    pos = position;
    //normal = norml;
    //normal = position; //I wrote this line after realizing normals don't work the way I thought/hoped they would work
    
    
    //Rotate, translate, aspect ratio?  or a different order?
    
    //Rotate:
    pos = rotationMatrixX * pos;
    normal = rotationMatrixX * normal;
    pos = rotationMatrixY * pos;
    normal = rotationMatrixY * normal;
    pos = rotationMatrixZ * pos;
    normal = rotationMatrixZ * normal;
    
    //translate:
    pos += xTrans;
    pos += yTrans;
    pos += zTrans;
    
    //Aspect ratio
    pos = aspectRatioMatrix * pos;
    gl_Position = vec4(pos, zoom);
}
