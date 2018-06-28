//Good reference for future: http://www.shaderific.com/glsl-functions/
//Want to make kinetic projectiles be orangish/redish, maybe with a flaming glow?

#version 410 core

in vec3 pos;
out vec4 color;

uniform float time;
uniform float zoom;
uniform float xTrans;
uniform float yTrans;
uniform float zTrans;
uniform float thetaX;
uniform float thetaY;
uniform float thetaZ;

//To get color RGB values, see: https://www.rapidtables.com/web/color/RGB_Color.html
vec3 dark_red = vec3(139.0f / 255.0f, 0.0f, 0.0f);
//vec3 brown = vec3(165.0f / 255.0f, 42.0f / 255.0f, 42.0f / 255.0f);
//vec3 saddle_brown = vec3(139.0f / 255.0f, 69.0f / 255.0f, 19.0f / 255.0f);
//vec3 sienna_brown = vec3(160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f);

void main() {
    //Here's a good one (and the one I am going to go with)
    if (time < 1.30f) { //0.01f * 120  (i.e. 6 seconds)
        vec3 fadingColor = vec3(1.65f*cos(1.65f * time), 0.35f*sin(1.15f*time), 0.15f + 0.1f*sin(time));
        color = vec4(max(fadingColor, dark_red), 1.0f);
    }
    else {
        //For constant color:
        //color = vec4(139.0/255.0f, 0.0f, 0.0f, 1.0f);
        
        //For Slowly fading color
        color = vec4(143.0f/255.0f - (time / 35.0f), 21.0f / 255.0f, 25.0f / 255.0f, 1.0f);
        
        
    }
        
   // color = vec4(sqrt(gl_FragCoord.y*gl_FragCoord.y + gl_FragCoord.x*gl_FragCoord.x),
   //                        sqrt(gl_FragCoord.y*gl_FragCoord.y - gl_FragCoord.x*gl_FragCoord.x),
   //                        sqrt(gl_FragCoord.x*gl_FragCoord.x - gl_FragCoord.y*gl_FragCoord.y),
   //                                                        1.0f);
    
    
    
    //color = vec4(1.0f - abs(sin(gl_FragCoord.x - xTrans + time)), 1.0f - abs(cos(gl_FragCoord.x - yTrans - time*xTrans)), 1.0f - abs (1.0 / pow(tan(xTrans*yTrans), 2.7f) ), 1.0f);
    
    //color = vec4(0.8f * tan(5.0f * time) - cos(pos.x * (gl_FragCoord.x - tan(gl_FragCoord.y))) + 0.25f * cos(10.0f * time), 0.5f + sin(time * pos.x * pos.y - pos.x), 0.75 * cos(-sin(0.45f * time + gl_FragCoord.z)), 1.0f );
}
