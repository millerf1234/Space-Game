#include "ProjectSetup.h" //Include this at the top of main

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cmath>

#include <unistd.h> //Only use on unix systems

#include "GLFW_Init.h"
#include "Game.h" //A wrapper to run the game inside of

int main(int argc, const char * argv[]) { //Add command line arg to open in windowed mode?
    
    char * dir = getcwd(NULL, 0); // Platform-dependent, Not Necessarily Portable
    printf("Current dir: %s\n\n", dir);
    
    std::cout << "Process Started... \n";
    
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
    std::cout << "Game Launched." << std::endl << std::endl;
    
    
    
    //Game.playIntroMovie(); //Implement later
    
    
    Game.launch(); //Launch will return only once the game has concluded its game loop
   
    windowSetupRoutines.terminate(); //Handle closing the window
    return EXIT_SUCCESS;
}
