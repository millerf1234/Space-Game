//  This is a file that just contains constants which affect all of the game parameters.
//  GameParameters.h
//
//  Created by Forrest Miller on 2/14/18.
//

#ifndef GameParameters_h
#define GameParameters_h

//Put all this in a namespace?
#include <string>

//-----------------------------------------------------------------------------
//   GAME PARAMETERS
//-----------------------------------------------------------------------------

static const char * NAME_OF_GAME = "Star Suzerian"; //Name of the game

static const bool curserVisible = false; //Experimental still...

constexpr float TIME_TICK_RATE = 0.01f; //Time increase per iteration

//-----------------------------------------------------------------------------
//   SYSTEM PARAMETERS   (Graphics Settings)
//-----------------------------------------------------------------------------

static const int DEFAULT_AA_SAMPLES = 4; //This is for global anti-aliasing done by GLFW
static const bool USE_VSYNC = true; //Should VSync be off/on
static const int DEFAULT_MONITOR = 1; //The primary monitor is 0, so 1 will be the next monitor after the primary monitor

static const int DEFAULT_OPENGL_VERSION_MAJOR = 4;
static const int DEFAULT_OPENGL_VERSION_MINOR = 1; 

//-----------------------------------------------------------------------------
//   FILE LOCATIONS  (For now they are all just .obj files)
//-----------------------------------------------------------------------------



static const char * SPACESHIP_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/space_ship2.obj";
static const char * ROCKET_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/Rocket.obj";


//-----------------------------------------------------------------------------
//   SHADER CODE LOCATIONS
//-----------------------------------------------------------------------------

//Background shaders
const std::string BACKGROUND_VERT_SHADER = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/background.vert";
const std::string BACKGROUND_FRAG_SHADER = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/background.frag";

//-----------------------------------------------------------------------------
//   Texture Image Locations (preferably JPEGs that are 915 x 609)
//-----------------------------------------------------------------------------
static constexpr int NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD = 1; //Increase this as more backgrounds are added

static constexpr int LEVEL_TO_LOAD = 1; //The level to load (Note I index valye down by 1 to match arrayIndex)

//Have array of background texture images
//But for now:
static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Cool_Picture_of_the_moon_915_609.jpg";
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/CalibrationPicture_915_609.jpg";


//    "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/CalibrationPicture_915_609.jpg",
//    "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/CalibrationPicture_915_609.jpg"


#endif /* GameParameters_h */
