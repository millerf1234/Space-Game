
#version 400 core

in vec3 pos;
uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;
uniform float red; //These uniforms are unique to player:
uniform float green;
uniform float blue;
out vec4 color;



//A fragment is like what I called my GridElem struct, in that it knows its position and its color and is like a pixel

//position specifies the position of a vertex
//pointSize specifies the size of a point (p.33)


void main() {
    //Orangeish/Yellowish/Redish lines
    //color = vec4(1.0f, 0.65f, 0.0f /* + 0.4f*sin(time3D / 5.0f)*/, 1.0f);
    
    color = vec4(red, green, blue, 1.0f);
    
    //Blue lines:
    //color = vec4(0.2f, 0.2f, 0.8f, 1.0f);
    
    //This next line makes the line colors shift ocasionally
    //color = vec4(abs(floor(sin(time3D * 2.0f))), abs(floor(sin(time3D + 3.1415927f/4.0f))), 0.25f *abs(floor(sin(time3D + 3.1415927f/2.0f))), 1.0f);
}

