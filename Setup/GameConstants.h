///  This file contains constant values that were not intended to be
///  changed without drastically affecting/breaking the game/program.
///  For tweakable constants, see the file GameParameters.h
///
///  File: GameConstants.h
///  Created by Forrest Miller on 6/1/18
///
///  Really Everything here was in place before 6/1/2018, I am just
///  finally moving them to be in their own file

#ifndef GAME_CONSTANTS
#define GAME_CONSTANTS

///Constant Values for Computations
static constexpr float PI = 3.1415927f;



///Application Constants
//OPENGL VERSION TO USE (HIGHLY RECCOMENDED THAT THIS NOT BE CHANGED FROM 4.1)
static const int DEFAULT_OPENGL_VERSION_MAJOR = 4;  //4.x
static const int DEFAULT_OPENGL_VERSION_MINOR = 1;  //x.1


///Model-Dependent Constants
//Don't change this next value from -2.94231f
static constexpr float PLAYER_ENGINE_FLAME_REAR_POSITION = -2.94231f;



///File Locations
/*
 //-----------------------------------------------------------------------------
 //   Release Version FILE LOCATIONS
 //-----------------------------------------------------------------------------
 
 // Why have 2 sets of file paths, you might ask? Since C++ is a compiled language,
 //  filepaths must be either known at compile time or passed in as command line
 //  arguments when the program launches. Since the avarage user will not be
 //  passing command line arguments, the only option left is to hard code in
 //  filepaths. Which brings us to the reason for having 2 different sets of file
 //  paths. When the application is launched from Xcode, the current directory
 //  of the application is not set to the same location as it is set to when
 //  launching the application outside of Xcode (i.e. as an archive). For
 //  example, when I launch the application from within Xcode on my Macbook Pro,
 //  the filepath is set to:
 //   /Users/forrestmiller/Library/Developer/Xcode/DerivedData/
 //    Star_Suzerian-dhfqccpgknzqbpeiriqmwjagqphn/Build/Products/Debug
 //
 //Unfortunatly, this filepath will be different for each computer that this
 // code is compiled/run on, so moving the project to a different computer will
 // involve/require changing every single filepath.
 //
 //The situation improves slightly when running the application as an archive
 // from outside of Xcode. Instead of the present working directory being set
 // to some buried debug folder, the filepath is set to:
 //  [harddrive/iCloud]/users/[user_name]
 // Luckily, this filepath contains the name of the User, which means it is
 // possible to find a static filepath that will remain the same on every computer.
 // From this directory, it is possible to directly access the machine's desktop.
 // Thus, for release versions, this application will look for a folder on the
 // user's desktop called "TestBundle", inside which can be contained all the
 // necessary files for the application. This works as long as there is a folder
 // on the desktop named 'TestBundle', but will cause the Application to fail
 // if this folder it every renamed/moved from the desktop.
 // There is a way to use Objective-C to determine filepaths at runtime, but I
 // have not yet done enough reading/experimenting to get this working correctly.
 //
 // For more information on the Objective-C filepath determination, see:
 //        https://stackoverflow.com/questions/7279100/c-ifstream-on-xcode-where-is-the-default-directory  (scroll down the page a bit)
 //   that link shows a description of a better way of doing this using
 //    Objective-C to get the actual filepath and then sending that into
 //    the C++ code.
 //
 // TL/DR: Depending on where application is launched, the starting working directory
 //        will change. Thus, there are different sets of filepaths that need
 //        to be switched if the application is launched outside of Xcode.
 //
 //        To launch outside of Xcode, make sure there is a folder on the desktop
 //        caled TestBundle that contains all the files (with matching names) to
 //        these filepaths. If any single one of these files is missing or not
 //        the name doesn't exactly match the name in the filepath, the
 //        application will not run correctly
 
 
 
 
 //NOTE!!! THESE FILEPATHS WON'T WORK IF PROJECT IS BUILT IN XCODE! INSTEAD,
 //        MUST ARCHIVE PROJECT AND THEN RUN THE ARCHIVE
 
 ///Objects
 static const char * SPACESHIP_MODEL_FILE_PATH = "/Desktop/TestBundle/space_ship2.obj";
 ///Shader Code
 const std::string BACKGROUND_VERT = "Desktop/TestBundle/Shaders/background.vert";
 const std::string BACKGROUND_FRAG = "Desktop/TestBundle/Shaders/background.frag";
 const std::string PLAYERSHIP_BODY_VERT = "Desktop/TestBundle/Shaders/PlayerShip/playership.vert";
 const std::string PLAYERSHIP_BODY_FRAG = "Desktop/TestBundle/Shaders/PlayerShip/playership.frag";
 const std::string PLAYERSHIP_LINE_VERT = "Desktop/TestBundle/Shaders/PlayerShip/playership.vert";///Note this is the same file as PLAYERSHIP_BODY_VERT
 const std::string PLAYERSHIP_LINE_FRAG = "Desktop/TestBundle/Shaders/PlayerShip/playershipLine.frag";
 const std::string PLAYERSHIP_ENGINE_VERT = "Desktop/TestBundle/Shaders/PlayerShip/engineEffect.vert";
 const std::string PLAYERSHIP_ENGINE_FRAG = "Desktop/TestBundle/Shaders/PlayerShip/engineEffect.frag";
 const std::string PLAYER_PARTICLE_VERT = "Desktop/TestBundle/Shaders/playerParticle.vert";
 const std::string PLAYER_PARTICLE_POINT_FRAG = "Desktop/TestBundle/Shaders/playerParticle.frag";
 const std::string PLAYER_PARTICLE_EXPLOSION_FRAG = "Desktop/TestBundle/Shaders/playerParticleExplosion.frag";
 const std::string KINETIC_VERT = "Desktop/TestBundle/Shaders/Weapons/kinetic.vert";
 const std::string KINETIC_FRAG = "Desktop/TestBundle/Shaders/Weapons/kinetic.frag";
 ///Image(s)
 static std::string backgroundTextureFP = "Desktop/TestBundle/Cool_Picture_of_the_moon.jpg";
 */


// /*
///-----------------------------------------------------------------------------
///   DEBUG (within Xcode) Version FILE LOCATIONS
///-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
///   MODEL FILE LOCATIONS  (For now they are all just .obj files)           ///
////////////////////////////////////////////////////////////////////////////////

static const char * SPACESHIP_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/space_ship2.obj";
static const char * ROCKET_MODEL_FILE_PATH = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ObjFiles/Rocket.obj";


////////////////////////////////////////////////////////////////////////////////
///   SHADER CODE LOCATIONS                                                  ///
////////////////////////////////////////////////////////////////////////////////

//Background shaders
const std::string BACKGROUND_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/background.vert";
const std::string BACKGROUND_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/background.frag";

//Player Ship Shaders
const std::string PLAYERSHIP_BODY_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/PlayerShip/playership.vert";
const std::string PLAYERSHIP_BODY_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/PlayerShip/playership.frag";
const std::string PLAYERSHIP_LINE_VERT = PLAYERSHIP_BODY_VERT; //Use same vert shader to keep aligned
const std::string PLAYERSHIP_LINE_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/PlayerShip/playershipLine.frag";
const std::string PLAYERSHIP_ENGINE_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/PlayerShip/engineEffect.vert";
const std::string PLAYERSHIP_ENGINE_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/PlayerShip/engineEffect.frag";

//Player Ship Particle (after death debris) Shaders
const std::string PLAYER_PARTICLE_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/playerParticle.vert";
const std::string PLAYER_PARTICLE_POINT_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/playerParticle.frag";
const std::string PLAYER_PARTICLE_EXPLOSION_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/playerParticleExplosion.frag";


//Weapon shaders
//Kinetic
const std::string KINETIC_VERT = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/Weapons/kinetic.vert";
const std::string KINETIC_FRAG = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/GLSL_Shaders/Weapons/kinetic.frag";

////////////////////////////////////////////////////////////////////////////////
///   Texture Image Locations  //see:http://img-resize.com/ to resize images ///
////////////////////////////////////////////////////////////////////////////////
static constexpr bool STAGE_POSITION_CENTER_AND_FULLSCREEN_IMAGE = false;  //Set to true depending on image that is loaded
static constexpr int NUMBER_OF_BACKGROUND_TEXTURES_TO_LOAD = 1; //Increase this as more backgrounds are added
static constexpr int LEVEL_TO_LOAD = 1; //The level to load (Note I index valye down by 1 to match arrayIndex)

//Have array of background texture images
//But for now:
//~~~~~~~~~~~~~~~~
//GOOD BACKGROUNDS
//~~~~~~~~~~~~~~~~
//Picture of the moon in original dimensions
static std::string backgroundTextureFP = "/Users/forrestmiller/Documents/Cool_Picture_of_the_moon.jpg";

//Another hubble photo
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/AnotherHubblePic.jpg";

//Picture of antartica:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Antarticata.jpg";

//~~~~~~~~~~~~~~~~
//Less GOOD BACKGROUNDS
//~~~~~~~~~~~~~~~~
// Hubble shot of the galaxy. Huge resolution! Has not been fixed to display properly. CAUSES MUCH LAG!
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Galaxy_history_revealed_by_the_Hubble_Space_Telescope_(GOODS-ERS2).jpg";

//Picture of jupiter I got from NASA JPL's website: https://photojournal.jpl.nasa.gov/catalog/PIA21774
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/JupiterPicture.jpg";

//~~~~~~~~~~~~~~~~
//Test BACKGROUNDS
//~~~~~~~~~~~~~~~~
//  Just the calibration Picture I used to fix the 915x609 resolution textures for them to display (usually) correctly.
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/CalibrationPicture_915_609.jpg";

//Another test picture I had on my laptop:
//static std::string backgroundTextureFP = "/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/ShaderImages/Mibra_mine.jpg";


#endif // GAME_CONSTANTS
