//  This is a file that just contains constants which affect all of the game parameters.
//  GameParameters.h
//
//
//  Edited continuously up through 3/20/2018 as needed with added functionality
//  Created by Forrest Miller on 2/14/18.
//

#ifndef GameParameters_h
#define GameParameters_h

//Put all this in a namespace?
#include <string>
#include "VectorOps.h" //I seem to be having problems with this getting included in multiple places,
//so by placing it here I should be safe since this class has no need to include other classes and is
//included almost everywhere

//To update github repository, do the following two (1 {only need second}) lines while in the main project folder:
//     git remote add origin https://github.com/millerf1234/Space-Game.git
//     git push -u origin master

static constexpr float PI = 3.1415927f;

//-----------------------------------------------------------------------------
// Debug Message Toggle Control (not complete)
//-----------------------------------------------------------------------------
static constexpr bool PRINT_DEBUG_WARNING_MESSAGES = false;
static constexpr bool PRINT_DEBUG_MESSAGES = false; //Turn off if not in debug mode
static constexpr bool PRINT_FRAME_PROCESS_TIME_INFORMATION = false; //Prints the process time for the frame to the console
static constexpr bool PRINT_MSAA_INFO_FROM_GPU_DRIVER = false; //This is just a test

static constexpr bool DRAW_COLLISION_DETAILS = true;

//-----------------------------------------------------------------------------
// Control Bindings
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ZOOM-IN PARAMETERS
// (Most Important!!!  Edit With Care!!!)
//-----------------------------------------------------------------------------
//Implement the zoom in at some point!
static constexpr int FRAMES_TO_HOLD = 500;
static constexpr float STARTING_ZOOM = 50.0f;
static constexpr float ENDING_ZOOM = 5.0f; //This is probably way to large a change

//-----------------------------------------------------------------------------
//   GAME PARAMETERS
//-----------------------------------------------------------------------------

//Faster Gameplay
//static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.045f;
//static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 1.00f;
////Rotation speed values are radians per frame
//static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/90.0f;//This means 95 frames to do full rotation (i.e. about 2 seconds)
//static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/25.0f; //So 30 frames to rotate 90 degrees

//Fast Gameplay
static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.032f; //Was 0.035f when game developed
static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 0.93f; //Was 0.95f
//Rotation speed values are radians per frame
static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/100.0f;//This means 95 frames to do full rotation (i.e. about 2 seconds)
static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/30.0f; //So 30 frames to rotate 90 degrees

//Slower Gameplay (comment Fast Gameplay and uncomment these)
//static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.015f; //Was 0.015f
//static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 0.45f; //Was 0.45f
////Rotation speed values are radians per frame
//static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/120.0f;//This means 120 frames to do full rotation (i.e. about 2 seconds)
//static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/17.0f; //So 30 frames to rotate 90 degrees


static constexpr float STARTING_PLAYER_HEALTH = 10.0f; //Tweak as needed
static constexpr float STARTING_PLAYER_SHIELDS = 4.0f; //Tweak as needed (if this gets implemented...)
static constexpr float STARTING_PLAYER_ENERGY = 25.0f; //Tweak as needed (if this ever gets implemented)
static constexpr float STARTING_PLAYER_FUEL = 100.0f; //Tweak freely, as this will most likely never be implemented
//Starting ammo:
static constexpr int STARTING_PLAYER_ROCKETS = 50; //TWEAK AS NEEDED PER GAMEPLAY CONDITIONS
static constexpr int STARTING_PLAYER_ROCKET_COUNT_MAX = 500; //TWEAK AS NEEDED
static constexpr int STARTING_PLAYER_HEXAGON_BOMBS = 6;

static constexpr float PLAYER_COLLISION_BOX_TO_MODEL_SCALE_FACTOR = 0.85f; //So the collision box is slightly smaller than the player model

//Player Starting Positions: (these start offsets were calculated with a PlayerSize of 75.0f
static constexpr float PLAYER1_STARTOFFSET_X = -65.0f;
static constexpr float PLAYER1_STARTOFFSET_Y = 40.0f;
static constexpr float PLAYER2_STARTOFFSET_X = 65.0f;
static constexpr float PLAYER2_STARTOFFSET_Y = -40.5f;
//I think I hardcoded in starting offsets for players beyond player 2, but currently controls are only implemented for 2 players...

//Edge of screen limits for player movement:
static constexpr float XLIMIT = 74.0f;
static constexpr float YLIMIT = 45.0f;

//Max Players (I currently have some things hardcoded for 2 players, so don't chnage this number)
static constexpr short MAX_PLAYERS = 2; //Shouldn't be set higher than 2... really don't do it....

static const char * NAME_OF_GAME = "SPACE ___! The Omega Occurance"; //Working Name of the game (not final)

static const bool curserVisible = false; //Experimental still... (i.e. it doesn't work at all)

//Game was written with an expected base TIME_TICK_RATE of 0.01f
constexpr float TIME_TICK_RATE = 0.01f; //Time step per loop iteration, tweak wisely...

//Player COLORS
//Player 1     (0.7f red, 0.48f green, 0.15f blue) is what I have been going with
static constexpr float PLAYER_ONE_RED = 0.7f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_ONE_GREEN = 0.48f; //Green amount
static constexpr float PLAYER_ONE_BLUE = 0.15f; //Blue amount

//Player 2     (0.3f red, 0.6f green, 0.85f blue) is what I have been going with
static constexpr float PLAYER_TWO_RED = 0.3f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_TWO_GREEN = 0.6f; //Green amount
static constexpr float PLAYER_TWO_BLUE = 0.85f; //Blue amount

//(Player 3) (if a player3 is ever added)  [Player3 will be GREEN]
static constexpr float PLAYER_THREE_RED = 0.2f; //Red amount on a 0.0f to 1.0f scale
static constexpr float PLAYER_THREE_GREEN = 1.0f; //Green amount
static constexpr float PLAYER_THREE_BLUE = 0.25f; //Blue amount

static constexpr float PLAYER_ENGINE_FLAME_SIZE_INCREASE_FROM_VELOCITY = 1.2f;

static constexpr bool DRAW_LINES_PLAYER_MODELS = true;

//-----------------------------------------------------------------------------
//   WEAPON PARAMETERS
//-----------------------------------------------------------------------------
//Lazer
static constexpr float LAZER_ENERGY_DRAIN = STARTING_PLAYER_ENERGY / 200.0f; //Gives 200 shots
static constexpr float LAZER_SPEED = 1.5f * PLAYER_MOVEMENT_MAX_SPEED; //0.8f;
static constexpr float LAZER_WIDTH = 0.05f;
static constexpr bool LAZER_COLOR_MATCH_PLAYER_COLOR = true;


//Hexagon Bomb
//static constexpr



//-----------------------------------------------------------------------------
//   SYSTEM PARAMETERS   (Graphics Settings)
//-----------------------------------------------------------------------------
static constexpr bool RUNNING_ON_MAC_MAC = true; //Does some overrides that are optimized specifically for my laptop

static const int DEFAULT_AA_SAMPLES = 2; //This is for global anti-aliasing done by GLFW (NOT SURE THIS DOES ANYTHING?)
static const bool USE_VSYNC = true; //Should VSync be off/on
static const int DEFAULT_MONITOR = 1; //The primary monitor is 0, so 1 will be the next monitor after the primary monitor
static const bool USE_FULLSCREEN = true;

//NOTE:I noticed that my macbook will still only run at 1080p if told to run at 4k, so these are usless if running on my laptop
static const bool USE_CUSTOM_RESOLUTION = false;   //Common display resolutions: 3840x2160 (4k),
static const int CUSTOM_DISPLAY_WIDTH = 3840;
static const int CUSTOM_DISPLAY_HEIGHT = 2160;

static constexpr float PLAYER_LINE_COLOR_BOOST_FACTOR = 1.5f; //Increases the color of the outline of the player ship models (from the base color of their body colors)

//OPENGL VERSION TO USE (HIGHLY RECCOMENDED THAT THIS NOT BE CHANGED FROM 4.1)
static const int DEFAULT_OPENGL_VERSION_MAJOR = 4;  //4.x
static const int DEFAULT_OPENGL_VERSION_MINOR = 1;  //x.1

//Suggested to leave player size at 75.0f, because other sizes will probably mess up collision detection
static constexpr float PLAYER_SIZE = 75.0f; //Larger numbers means smaller player size
static constexpr int PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES = 18; //~15 is a good value

//Sample points currently unused
static constexpr int COLLISION_SAMPLE_POINTS = 10; //Must be multiple of 2, should be 10 or greater

//Don't change this next value from -2.94231f
constexpr float PLAYER_ENGINE_FLAME_REAR_POSITION = -2.94231f;

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
