
#version 410 core

//in vec4 position;
in vec3 position;

uniform float time;
uniform float red;
uniform float green;
uniform float blue;
uniform float zoom;
uniform float pointSize;

//out vec4 colorShift;

mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
                              0.0f, 2650.0f/1600.0f, 0.0f,
                              0.0f,       0.0f     , 1.0f );

//mat3 rotationMatrixZ = mat3(    cos(time)    ,     -sin(time)    ,      0.0f          ,
//                                sin(time)    ,      cos(time)    ,      0.0f          ,
//                                 0.0f        ,        0.0f       ,      1.0f          );

////Normal main
void main()
{
    gl_PointSize = pointSize; // 1.0f;
    //vec3 pos = position.xyz;
    vec3 pos = position;

   // pos = rotationMatrixZ * rotationMatrixZ * pos;
    

    //Aspect ratio
    pos = aspectRatioMatrix * pos;

    //pos.x += 0.035*sin(time*25.0*sqrt(pow(3.0f*sin(pos.x), 2.0) + pow(3.0f*sin(pos.y), 2.0)));
    //pos.y += 0.035*cos(time*25.0*sqrt(pow(pos.x, 2.0) + pow(pos.y, 2.0)));

    //gl_Position = vec4(pos, position.w);
    gl_Position = vec4(pos, zoom);

}






/////Messing around main  (makes particles move in a wave-like effect
//void main()
//{
//    gl_PointSize = pointSize; // 1.0f;
//    //vec3 pos = position.xyz;
//    vec3 pos = position;
//
//    //pos = rotationMatrixZ * rotationMatrixZ * pos;
//
//    //Aspect ratio
//    pos = aspectRatioMatrix * pos;
//
//    pos.x += 0.2055*sin(time+time/7.7)*sqrt(pow(pos.x, 2.0) + pow(pos.y, 2.0));
//    pos.y += 0.2055*cos(time/7.7)*sqrt(pow(pos.x, 2.0) + pow(pos.y, 2.0));
//
//
//    //gl_Position = vec4(pos, position.w);
//    gl_Position = vec4(pos, zoom);
//
//}





//I realized that doing instanced drawing is pointless (no-pun-intended) when drawing
//points. Also, the triangles will be different sizes, so those don't count either...
////I am going to attempt to write this so it works by drawing instances
////  This is based off example on page 264 of OpenGL SuperBible
//#version 410 core
//
//in vec4 position;
//in vec4 color;
//in vec4 instanceOffset;
//
//uniform float time;
//
//
//out vec4 Fragment {
//    vec4 color;
//} fragment;
//
//
//mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
//                              0.0f, 2650.0f/1600.0f, 0.0f,
//                              0.0f,       0.0f     , 1.0f );
//
//void main()
//{
//    vec3 pos = position.xyz;
//
//    //Aspect ratio
//    pos = aspectRatioMatrix * pos;
//
//    gl_Position = vec4(pos, position.w);
//    fragment.color = color;
//}
//
//



//#version 400 core
//
//layout(location = 0) in vec3 position;
//
//uniform float time;
//uniform float zoom;
//uniform float red;
//uniform float green;
//uniform float blue;
//
//out vec3 pos;
//
//
//void main()
//{
//    pos = position;
//
//    //Aspect ratio
//    pos = aspectRatioMatrix * pos;
//    gl_Position = vec4(pos, zoom);
//}
