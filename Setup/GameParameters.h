//  This is a file that just contains constants which affect all of the game parameters.
//  GameParameters.h
//
//  Edited continuously up through 2/20/2018 as needed with added functionality
//  Created by Forrest Miller on 2/14/18.
//

#ifndef GameParameters_h
#define GameParameters_h

//Put all this in a namespace?
#include <string>

//-----------------------------------------------------------------------------
// ZOOM-IN PARAMETERS
// (Most Important!!!  Edit With Care!!!)
//-----------------------------------------------------------------------------
static constexpr int FRAMES_TO_HOLD = 500;
static constexpr float STARTING_ZOOM = 50.0f;
static constexpr float ENDING_ZOOM = 5.0f; //This is probably way to large a change


//-----------------------------------------------------------------------------
//   GAME PARAMETERS
//-----------------------------------------------------------------------------

static constexpr float STARTING_PLAYER_HEALTH = 10.0f; //Tweak as needed
static constexpr float STARTING_PLAYER_SHIELDS = 4.0f; //Tweak as needed (if this gets implemented...)
static constexpr float STARTING_PLAYER_ENERGY = 25.0f; //Tweak as needed (if this ever gets implemented)
static constexpr float STARTING_PLAYER_FUEL = 100.0f; //Tweak freely, as this will most likely never be implemented
static constexpr float STARTING_PLAYER_ROCKETS = 50.0f; //TWEAK AS NEEDED PER GAMEPLAY CONDITIONS
static constexpr float STARTING_PLAYER_ROCKET_COUNT_MAX = 500.0f; //TWEAK AS NEEDED


static const char * NAME_OF_GAME = "SPACE ___! The Omega Occurance"; //Working Name of the game (not final)

static const bool curserVisible = false; //Experimental still... (i.e. it doesn't work)

constexpr float TIME_TICK_RATE = 0.01f; //Time step per loop iteration, tweak wisely...


//Player COLORS
//Player 1
static constexpr float PLAYER_ONE_RED = 0.3f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_ONE_GREEN = 0.35f; //Green amount
static constexpr float PLAYER_ONE_BLUE = 0.8f; //Blue amount
//Player 2
static constexpr float PLAYER_TWO_RED = 0.9f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_TWO_GREEN = 0.25f; //Green amount
static constexpr float PLAYER_TWO_BLUE = 0.15f; //Blue amount
//(Player 3) (if a player3 is ever added)
static constexpr float PLAYER_THREE_RED = 0.2f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_THREE_GREEN = 1.0f; //Green amount
static constexpr float PLAYER_THREE_BLUE = 0.25f; //Blue amount

//-----------------------------------------------------------------------------
//   SYSTEM PARAMETERS   (Graphics Settings)
//-----------------------------------------------------------------------------

static constexpr short MAX_PLAYERS = 2; //Shouldn't be set higher than 2... really don't do it....

static const int DEFAULT_AA_SAMPLES = 4; //This is for global anti-aliasing done by GLFW
static const bool USE_VSYNC = true; //Should VSync be off/on
static const int DEFAULT_MONITOR = 1; //The primary monitor is 0, so 1 will be the next monitor after the primary monitor


//OPENGL VERSION TO USE (HIGHLY RECCOMENDED THAT THIS NOT BE CHANGED FROM 4.1)
static const int DEFAULT_OPENGL_VERSION_MAJOR = 4;  //4.x
static const int DEFAULT_OPENGL_VERSION_MINOR = 1;  //x.1

//-----------------------------------------------------------------------------
//   FILE LOCATIONS  (For now they are all just .obj files)
//-----------------------------------------------------------------------------

static const char * SPACESHIP_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/space_ship2.obj";
static const char * ROCKET_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/Rocket.obj";


//-----------------------------------------------------------------------------
//   SHADER CODE LOCATIONS
//-----------------------------------------------------------------------------

//Background shaders
const std::string BACKGROUND_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/background.vert";
const std::string BACKGROUND_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/background.frag";

//Player ship Shaders
const std::string PLAYERSHIP_BODY_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/PlayerShip/playership.vert";
const std::string PLAYERSHIP_BODY_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/PlayerShip/playership.frag";
const std::string PLAYERSHIP_LINE_VERT = PLAYERSHIP_BODY_VERT; //Use same vert shader to keep aligned
const std::string PLAYERSHIP_LINE_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/PlayerShip/playershipLine.frag";
const std::string PLAYERSHIP_ENGINE_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/PlayerShip/engineEffect.vert";
const std::string PLAYERSHIP_ENGINE_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/Shaders/PlayerShip/engineEffect.frag";


//-----------------------------------------------------------------------------
//   Texture Image Locations (preferably JPEGs that are 915 x 609)    //see: http://img-resize.com/   to resize images
//-----------------------------------------------------------------------------
static constexpr int NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD = 1; //Increase this as more backgrounds are added

static constexpr int LEVEL_TO_LOAD = 1; //The level to load (Note I index valye down by 1 to match arrayIndex)

//Have array of background texture images
//But for now:

// Hubble shot of the galaxy. Huge resolution! Has not been fixed to display properly
// from:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Galaxy_history_revealed_by_the_Hubble_Space_Telescope_(GOODS-ERS2).jpg";
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/HubbleGalaxyHistory_915_609.jpg"; //I fixed to display properly, but it's sorta distorted now


// Picture of the moon. Picture is the correct dimensions to be fixed to display properly.
static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Cool_Picture_of_the_moon_915_609.jpg";

//
////Picture of pluto:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Pluto_4000_4000_JPEG.jpeg";


//Picture of antartica:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Antarticata.jpg";

//Another test picture:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Mibra_mine.jpg";


//  Just the calibration Picture I used to fix the 915x609 resolution textures for them to display (usually) correctly.
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/CalibrationPicture_915_609.jpg";


#endif /* GameParameters_h */
