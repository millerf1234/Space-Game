
#version 410 core

//in vec4 position;
in vec3 position;

uniform float time;
uniform float red;
uniform float green;
uniform float blue;
uniform float zoom;


//out vec4 colorShift;

mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
                              0.0f, 2650.0f/1600.0f, 0.0f,
                              0.0f,       0.0f     , 1.0f );

void main()
{
    gl_PointSize = 20.0f;
    //vec3 pos = position.xyz;
    vec3 pos = position;
    
    //Aspect ratio
    pos = aspectRatioMatrix * pos;
    
    //gl_Position = vec4(pos, position.w);
    gl_Position = vec4(pos, zoom);
    
}





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
