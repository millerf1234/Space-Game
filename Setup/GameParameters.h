//  This is a file that just contains constants which affect all of the game parameters.
//  GameParameters.h
//
//
//  Edited continuously up through 3/20/2018 as needed with added functionality
//                     Actually make that through 5/30/2018 
//  Created by Forrest Miller on 2/14/18.
//

//-----------------------------------------------------------------------------
//(On Mac) To TEST KEYBOARD INPUT FUNCTIONALITY, OPEN CONSOLE AND TYPE:
//  open -a KeyboardViewer
//-----------------------------------------------------------------------------

#ifndef GAME_PARAMETERS
#define GAME_PARAMETERS

//Put all this in a namespace?
#include <string>
#include "GameConstants.h"  //Only include GameConstants.h once per project, ideally in this file


//To update github repository, do the following two (1 {only need second}) lines while in the main project folder:
//     git remote add origin https://github.com/millerf1234/Space-Game.git
//     git push -u origin master



//NOTE!  IN Generator under the function./ delete element, I set the value in the seperate array of element ID's that I track
//       to some bogus large negative number to represent that that instance was deleted, this way I am not constantly
//       resizing the array... wait... what?


//-----------------------------------------------------------------------------
//                       OPEN FULL SCREEN OR WINDOWED
//-----------------------------------------------------------------------------
static constexpr bool USE_FULLSCREEN = true;


//-----------------------------------------------------------------------------
// Debug Message Toggle Control (not complete)
//-----------------------------------------------------------------------------
static constexpr bool PRINT_DEBUG_WARNING_MESSAGES = true;
static constexpr bool PRINT_DEBUG_MESSAGES = false; //Turn off if not in debug mode
static constexpr bool PRINT_FRAME_PROCESS_TIME_INFORMATION = false; //Prints the process time for the frame to the console  (question: How does out-of-order execution optimization affect the timing functions? Are they always guarenteed to execute at the point they appear in the code?)
static constexpr bool PRINT_MSAA_INFO_FROM_GPU_DRIVER = false; //This is just a test (it looks like my laptop refuses to allow MSAA)
static constexpr bool PRINT_DESTRUCTOR_CALLS = false; //For debug (doesn't print every destructor call)
static constexpr bool PRINT_WEAPON_SPAWN_POINT_COORDS = false; //More debug stuff
static constexpr bool PRINT_WEAPON_SPAWN_ANGLES = false; //Mostly for debug
static constexpr bool PRINT_PLAYER_PARTICLES_DRAW_COUNTS = false;


//-----------------------------------------------------------------------------
// Game Message (Non-Debug) Toggle(s)
//-----------------------------------------------------------------------------
static constexpr bool PRINT_PLAYER_DAMAGE_MESSAGES = true; //Print a message each time a player takes damage

//-----------------------------------------------------------------------------
// Draw Options
//-----------------------------------------------------------------------------
static constexpr bool DRAW_COLLISION_DETAILS = false;

static constexpr bool DRAW_MODELS = true;

static constexpr bool PLAY_DEATH_ANIMATION = true;


//-----------------------------------------------------------------------------
// Gamemode types/Options
//-----------------------------------------------------------------------------
static constexpr bool MOON_DRIFTER_MODE = false;
//Battle mode by default

static constexpr bool LIMIT_PLAYER_WEAPON_FIRE_RATE = true;

//-----------------------------------------------------------------------------
// Moondrifter constants
//-----------------------------------------------------------------------------
constexpr short mdSCORE_TO_WIN = 100;
constexpr short mdSCORE_PER_LAP = 1;

constexpr float mdPLAYER1SPAWNXOFFSET = -10.0f;
constexpr float mdPLAYER1SPAWNYOFFSET = 20.0f;
constexpr float mdPLAYER2SPAWNXOFFSET = 30.0f;
constexpr float mdPLAYER2SPAWNYOFFSET = -23.0f;

constexpr float mdPLAYER1SPAWNROTATION =  PI;
constexpr float mdPLAYER2SPAWNROTATION = 0.0f;

//Left inner circle
constexpr float mdINNERZONE_LEFT_CENTER_X = -20.0f;
constexpr float mdINNERZONE_LEFT_CENTER_Y = 0.0f;
constexpr float mdINNERZONE_LEFT_STARTRADIUS = 30.0F;
constexpr float mdINNERZONE_LEFT_LOWERTHETA = - PI / 7.0f;
constexpr float mdINNERZONE_LEFT_UPPERTHETA = PI / 7.0f;

//Right inner circle
constexpr float mdINNERZONE_RIGHT_CENTER_X = -20.0f;
constexpr float mdINNERZONE_RIGHT_CENTER_Y = 0.0f;
constexpr float mdINNERZONE_RIGHT_STARTRADIUS = 30.0F;
constexpr float mdINNERZONE_RIGHT_LOWERTHETA = - PI / 7.0f;
constexpr float mdINNERZONE_RIGHT_UPPERTHETA = PI / 7.0f;

//OUTER CIRCLE
constexpr float mdOUTERZONE_CENTER_X = -20.0f;
constexpr float mdOUTERZONE_CENTER_Y = 0.0f;
constexpr float mdOUTERZONE_STARTRADIUS = 30.0F;
constexpr float mdOUTERZONE_LOWERTHETA = - PI / 7.0f;
constexpr float mdOUTERZONE_UPPERTHETA = PI / 7.0f;


//-----------------------------------------------------------------------------
// Control Bindings
//-----------------------------------------------------------------------------
// To do... Right now the controls are hardcoded in PlayerManager (with pause/escape defined within Game.cpp)

//-----------------------------------------------------------------------------
// ZOOM-IN PARAMETERS
// (Most Important!!!  Edit With Care!!!)
//-----------------------------------------------------------------------------
//Implement the zoom in at some point! Once a win condition has been implemented
static constexpr int FRAMES_TO_HOLD = 500;
static constexpr float STARTING_ZOOM = 50.0f;
static constexpr float ENDING_ZOOM = 5.0f; //This is probably way to large a change

//-----------------------------------------------------------------------------
//   GAME PARAMETERS
//-----------------------------------------------------------------------------
////Faster Gameplay
//static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.045f;
//static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 1.00f;
////Rotation speed values are radians per frame
//static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/90.0f;//This means 95 frames to do full rotation (i.e. about 2 seconds)
//static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/25.0f; //So 30 frames to rotate 90 degrees

//Fast Gameplay (Normal)
static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.03225f; //Was 0.035f when game developed  //was also 0.032f for a while
static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 0.9367f; //Was 0.95f //Was also 0.93f
//Rotation speed values are radians per frame                 // 100.0f for rotation turning was old value
static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/90.0f;//This means 95 frames to do full rotation (i.e. about 2 seconds)
static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/30.0f; //So 30 frames to rotate 90 degrees

////Slower Gameplay (comment Fast Gameplay and uncomment these)
//static constexpr float PLAYER_MOVEMEMT_ACCELERATION_LINEAR = 0.015f; //Was 0.015f
//static constexpr float PLAYER_MOVEMENT_MAX_SPEED = 0.45f; //Was 0.45f
////Rotation speed values are radians per frame
//static constexpr float PLAYER_ROTATION_SPEED_TURNING = (2.0f*PI)/120.0f;//This means 120 frames to do full rotation (i.e. about 2 seconds)
//static constexpr float PLAYER_ROTATION_SPEED_ROLLING = (PI/2.0f)/17.0f; //So 30 frames to rotate 90 degrees

//Suggested to leave scale at 75.0f. Changing GAME_SCALE will cause all models to be resized (except for the background)
static constexpr float GAME_SCALE = 75.0f; //Larger numbers means smaller player size
static constexpr int PLAYER_ENGINE_FLAME_TRANSLATION_DELAY_FRAMES = 18; //~15 is a good value

static constexpr int FRAMES_BETWEEN_PLAYER_RESPAWN = 180; //3 sec is ~180 frames

static constexpr float STARTING_PLAYER_HEALTH = 1.0f; //Tweak as needed
static constexpr float STARTING_PLAYER_SHIELDS = 4.0f; //Tweak as needed (if this gets implemented...)
static constexpr float STARTING_PLAYER_ENERGY = 25.0f; //Tweak as needed (if this ever gets implemented)
static constexpr float STARTING_PLAYER_FUEL = 100.0f; //Tweak freely, as this will most likely never be implemented
//Starting ammo:

static constexpr int STARTING_PLAYER_ROCKETS = 50; //TWEAK AS NEEDED PER GAMEPLAY CONDITIONS
static constexpr int STARTING_PLAYER_ROCKET_COUNT_MAX = 500; //TWEAK AS NEEDED
static constexpr int STARTING_PLAYER_HEXAGON_BOMBS = 6;

static constexpr float PLAYER_COLLISION_BOX_TO_MODEL_SCALE_FACTOR = 0.85f; //So the collision box is slightly smaller than the player model

///Player Starting Positions: (these start offsets were calculated with a PlayerSize of 75.0f)
static constexpr float PLAYER1_STARTOFFSET_X = -65.0f * (GAME_SCALE / 75.0f);
static constexpr float PLAYER1_STARTOFFSET_Y = 40.0f * (GAME_SCALE / 75.0f);
static constexpr float PLAYER2_STARTOFFSET_X = 65.0f * (GAME_SCALE / 75.0f);
static constexpr float PLAYER2_STARTOFFSET_Y = -40.5f * (GAME_SCALE / 75.0f);
//I think I hardcoded in starting offsets for players beyond player 2, but currently controls are only implemented for 2 players...


//Edge of screen limits for player movement: (Suggested to leave these at 74.0f, 45.0f and change GAME_SCALE instead)
static constexpr float XLIMIT = 74.0f*(GAME_SCALE/75.0f); //Divide by 75 to scale screen limits with changes to GAME_ENTITY_SCALE
static constexpr float YLIMIT = 45.0f*(GAME_SCALE/75.0f); //Divide by 75 to scale screen limits with changes to GAME_ENTITY_SCALE;

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

static constexpr float PLAYER_ENGINE_FLAME_SIZE_INCREASE_FROM_VELOCITY = 1.6f; //1.2f is a good value

static constexpr bool DRAW_LINES_PLAYER_MODELS = true;

//-----------------------------------------------------------------------------
//   WEAPON PARAMETERS
//-----------------------------------------------------------------------------
//Laser
//static constexpr float LASER_ENERGY_DRAIN = STARTING_PLAYER_ENERGY / 200.0f; //Gives 200 shots
//static constexpr float LASER_SPEED = 1.5f * PLAYER_MOVEMENT_MAX_SPEED; //0.8f;
//static constexpr float LASER_WIDTH = 0.05f;
//static constexpr bool LASER_COLOR_MATCH_PLAYER_COLOR = true;

//Kinetic
static constexpr float PROJECTILE_SIZE = 6.0f; //Larger number means smaller. I would say about 4.0f
//For velocity calculation of Kinetic see KineticWeaponManager.cpp around line 210
static constexpr float KINETIC_SPEED_FACTOR = 1.95f; //1.75f is good value, formula is (playerShip'sSpeed) + KINETIC_SPEED_FACTOR * PLAYER_MAX_SPEED

static constexpr int PLAYER_KINETIC_AMMO = 600000; //Give a lot until I add picking up ammo
static constexpr float KINETIC_WEP_DAMAGE = 1.0f; //0.01f;

static constexpr float KINETIC_VELOCITY_IMPACT = 0.04f;//0.04f; //0.04
//static constexpr float KINETIC_VELOCITY_LAUNCH_IMPULSE = 1.0f;

constexpr int KINTETIC_PROJECTILES_BURST_SHOTS_BEFORE_COOLDOWN = 5; //5-9
//Hexagon Bomb
//static constexpr

//-----------------------------------------------------------------------------
//   SYSTEM PARAETERS   (Graphics Settings)
//-----------------------------------------------------------------------------
static constexpr bool RUNNING_ON_MY_MAC = true; //Does some overrides that are optimized specifically for my laptop

static const int DEFAULT_AA_SAMPLES = 2; //This is for global anti-aliasing done by GLFW (NOT SURE THIS DOES ANYTHING?)
static const bool USE_VSYNC = true; //Should VSync be off/on
static const int DEFAULT_MONITOR = 1; //The primary monitor is 0, so 1 will be the next monitor after the primary monitor

//NOTE:I noticed that my macbook will still only run at 1080p if told to run at 4k, so these are usless if running on my laptop
static const bool USE_CUSTOM_RESOLUTION = false;   //Common display resolutions: 3840x2160 (4k),
static const int CUSTOM_DISPLAY_WIDTH = 3840; //1920
static const int CUSTOM_DISPLAY_HEIGHT = 2160; //1080

static constexpr float PLAYER_LINE_COLOR_BOOST_FACTOR = 1.5f; //Increases the color of the outline of the player ship models (from the base color of their body colors)


//Collision Sample points currently unused because my advanced collision algorithm never worked correctly
static constexpr int COLLISION_SAMPLE_POINTS = 10; //Must be multiple of 2, should be 10 or greater


static constexpr int PLAYER_EXPLOSION_PARTICLES_PER_WAVE = 310; //143
static constexpr int FRAMES_BETWEEN_PLAYER_EXPLOSION_WAVE = 19;  //1 or 2?
static constexpr int EXPLOSION_PARTICLE_FRAMES_CUTOFF = 190;//50; //Divide by frames between explosion wave to get the number of waves that will be generated
static constexpr float EXPLOSION_PARTICLE_POINT_SIZE = 2.95f;
static constexpr float PLAYER_PARTICLE_POINT_SIZE = 0.95f;

static constexpr bool MULTIPLE_EXPLOSION_PARTICLES_PER_CALL = true;

static constexpr int FRAMES_TO_KEEP_PLAYER_EXPLOSION_AROUND = 405; //540;


#endif // GAME_PARAMETERS
