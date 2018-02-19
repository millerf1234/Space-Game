//
//  This is just a header that includes necessary dependencies and what not that
//  I need to get everything up and running. This headder should only be included
//  once at the top of main. Including it twice will cause problems.
//
//  ProjectSetup.h
//  OpenGLSetupTest
//
//  Created by Forrest Miller on 1/15/18.
//  Copyright © 2018 Forrest Miller. All rights reserved.
//

// Note on framework versioning: When you build a new version you must update "Current Library Version". There is also a "Compatibility Version"
//In Xcode selector your framework target and in build settings search for linking and you will se this setting there.
//We created a build script to bump this version number every time we build.
//Xcode caches the frameworks and uses the cache if possible. That's why you need to change this number.
//   from : https://stackoverflow.com/questions/6615494/how-to-update-a-framework-in-an-ios-app




#ifndef ProjectSetup_h
#define ProjectSetup_h
//#pragma once

#include <fstream>
#include <string>
#include <iostream>

// System Headers
#include "glad.h"
#include "glfw3.h"
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
//#include <btBulletDynamicsCommon.h>
//#define GLEW_STATIC
//#include <GL/glew.h> //Guess I don't need GLEW?
//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <GLFW/glfw3.h>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h" //Have it so that any class that needs stb_image includes this file in their .cpp file (and not their headder file)

//NOTE THAT WIDTH AND HEIGHT ARE NOW BEING DETECTED WITHIN GLFW_Init
//const int mWidth = 1670; // or 1920
//const int mHeight = 960; // or 1080
//Note that 1670x960 is the resolution to use on my 13" macbook pro when running
//in windowed mode. 

#endif /* ProjectSetup_h */
