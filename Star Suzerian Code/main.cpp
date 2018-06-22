#include "ProjectSetup.h" //Include this at the top of main

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <sys/resource.h>

#include <unistd.h> //Only use on unix systems

#include "GLFW_Init.h"
#include "Game.h" //A wrapper to run the game inside of

#include "AudioRenderer.h"
#include "AudioData.h"


int main(int argc, const char * argv[]) { //Add command line arg to open in windowed mode?
    
    ///DEBUG CODE: DELETE AT SOME POINT...
    //AudioRenderer * test = new AudioRenderer();
    //delete test;
    AudioData * data = new AudioData("/Users/forrestmiller/Desktop/xcode_test_projects/Star Suzerian/WAV_Files/Pulse_gun_05.wav");
    
    
    
    char * dir = getcwd(NULL, 0); // Platform-dependent, Not Necessarily Portable. This is more to help me figure out where to load files from by getting the directory where the executable will be looking for files. The result of this will vary depending on where the program was launched from.
    printf("Current dir: %s\n\n", dir);
    
    std::cout << "\nThis program was compiled using:\n    ";
#if defined __GNUC__
    //see: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
    std::cout << "GNU C/C++ " << __VERSION__  /* << " compiler!"*/ << std::endl;
#elif defined __llvm__
#if defined __clang__
    std::cout << "LLVM CLANG " << LLVM_VERSION_STRING << std::endl;
#else
    std::cout << "LLVM with unknown front-end (i.e. front end is not clang)" << std::endl;
#endif //if defined __llvm__
#else
    std::cout << "\"A compiler that I did not include a pre-processor directive for...\"" << std::endl;
#endif //#ifdef __GNUC__    elif def
    
    std::cout << "\n\nProcess Started... \n";
    
    std::cout << "Initializing Window: " << std::endl;
    
    GLFW_Init windowSetupRoutines; //Create a GLFW_Init object to handle window creation
    //windowSetupRoutines.openWindowed();//Uncomment this line to open program in windowed mode
    windowSetupRoutines.setDefaultMonitor(DEFAULT_MONITOR);//Monitors start at 0, so 1 is a secondary display plugged in
    MonitorData detectedDisplayInfo = windowSetupRoutines.initialize(); //Run window creation routines
    
    if (detectedDisplayInfo.validContext != true) { //If something went wrong in the window setup routines
        std::cout << "\n\nAn error occured while setting the context for the display. Program will now crash... \n";
        return EXIT_FAILURE;
    }
    
    //Load OpenGL functions once window context has been set
    std::cout << "\nLoading Graphics Language..." << std::endl;
    gladLoadGL();
    
    fprintf(stderr, "    Graphics Language loaded.\n    Graphics Language version: OpenGL %s", glGetString(GL_VERSION));
    //set OpenGl global state parameters
    fprintf(stderr, "\nInitializing GL StateMachine...\n");
    std::cout << "    Activating GL depth-test\n";
    glEnable(GL_DEPTH_TEST); //Turn on the depth test for z-culling
    //std::cout << "    Activating GL scissor-test\n";
    //glEnable(GL_SCISSOR_TEST);
    
    
    //looks like glGetString comes with a terminating new line. To remove this newline, do:
    //      str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());  //where str is a string
    
    std::cout << std::endl << std::endl << "Preparing To Launch Game..." << std::endl;
    
    std::cout << "    Generating a new instance of the game..." << std::endl;
    Game Game(detectedDisplayInfo); //Create an instance of the game and tell
    //the instance about all the monitor setup that has been done thus far.
    
    
    
//    //How I initially imagined loading would work:
//    std::cout << "Done\n    Loading Models...";
//    Game.loadGameObjects();
//    std::cout << "Done\n    Loading Shaders...";
//   // Game.loadShaders();
//    std::cout << "Done\n    Loading Textures...";
//    //Game.loadTextures();
//    std::cout << "Done\n" << std::endl;
//    std::cout << "Game Launched." << std::endl << std::endl;
   
    
    
    //How loading actually works:
    std::cout << "\n    Loading Game Assets...";
    Game.loadGameObjects();
    std::cout << "    Finished Loading Game Assets.\n" << std::endl;
    
    ///As you can see in this next line, I lie... The game clearly has not launched when
    ///I print out the message saying it has.
    std::cout << "Game Launched." << std::endl << std::endl;
    
    //Game.playIntroMovie(); //Implement later (when budget is bigger)
    
    Game.launch(); ///Now the game has launched
    //Launch will return only once the game has concluded its game loop
    windowSetupRoutines.terminate(); //Handle closing the window
    return EXIT_SUCCESS;
}
