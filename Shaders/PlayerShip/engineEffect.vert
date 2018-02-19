

#version 400 core

//Currently I am using the same vert shader for both the main and side engines

in vec3 enginePos;
//in vec2 engineTxtr;

uniform float zoom;
uniform float time;

out vec3 enginePosition;
//out vec2 engineTex;


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


//mat3 rotationMatrixX = mat3(    1.0f  ,      0.0f    ,     0.0f     ,
//                            0.0f  , cos(dg2Rad(90.0f)) , -sin(dg2Rad(90.0f)),
//                            0.0f  , sin(dg2Rad(90.0f)) ,  cos(dg2Rad(90.0f)));


void main()
{
    enginePosition = enginePos;
    // engineTex = engineTxtr;
    
    //        mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
    //                                      0.0f, 2650.0f/1600.0f, 0.0f,
    //                                      0.0f,       0.0f     , 1.0f );
    
    
    
    enginePosition = enginePosition + vec3(0.0f, -1.75f /* * sin(time3D)*/, 0.0f);
    //enginePosition = aspectRatioMatrix * rotationMatrixX * enginePosition;
    enginePosition = aspectRatioMatrix * enginePosition;
    
    //enginePosition.x += 0.005f*cos(time*100.0f);
    
    
    gl_Position = vec4(enginePosition, 40.5 + zoom) ;// vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
}
