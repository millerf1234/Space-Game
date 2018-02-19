//This is just a debug shader for testing out non-rendering parts of this program
#version 400 core

layout(location = 0) in vec2 position;
//layout(location = 1) in vec3 colr;
//layout(location = 2) in vec2 texCoord;

//uniform float time;
//uniform float tex;
//uniform float zoom;



out vec2 pos;
out vec3 COLR;
out vec2 TexCoor;

//vec3 BACKGROUND_OFFSET = vec3(0.0f, 0.0f, 1.0f);

mat3 aspectRatioMatrix = mat3(1.0f,       0.0f     , 0.0f,
                              0.0f, 1920.0f/1080.0f, 0.0f,
                              0.0f,       0.0f     , 1.0f );

void main()
{
    //pos = position;
    COLR = vec3(0.25f, 0.55f, 0.59f);
    TexCoor = vec2(0.0f, 0.0f);
    
    //pos = vec3( aspectRatioMatrix * vec3(position.x, position.y, 0.1f));
    
    vec3 temp = vec3( aspectRatioMatrix * vec3(position.x, position.y, 0.0f));
    
    
    //gl_Position = vec4(temp /*+ vec3(xCoor, yCoor, 0.0f)*/, 1.0 + zoom);
    
    //gl_Position = vec4(pos, -1.5);
    //gl_Position = vec4(position.x, position.y, 0.0, 1);
    gl_Position = vec4(temp, 20.0f);
    
    //gl_PointSize = 10 + sin(time);
}

