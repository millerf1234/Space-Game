#include "ProjectSetup.h" //Include this at the top of main

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "GLFW_Init.h"
#include "GameObj.h" //A wrapper to run the game inside of

const int defaultMonitor = 1; //0 for primary display, 1 for secondary display, 2-inf for additional displays

int main(int argc, const char * argv[]) { //Add command line arg to open in windowed mode?
    
    std::cout << "Process Started... \n";
    
    std::cout << "Initializing Window: " << std::endl;
    
    GLFW_Init windowSetupRoutines; //Create a GLFW_Init object to handle window creation
    //windowSetupRoutines.openWindowed();//Uncomment this line to open program in windowed mode
    windowSetupRoutines.setDefaultMonitor(1);//Monitors start at 0, so 1 is a secondary display plugged in
    Monitors detectedDisplayInfo = windowSetupRoutines.initialize(); //Run window creation routines
    
    //Load OpenGL functions once window context has been set
    std::cout << "\nLoading Graphics Language..." << std::endl;
    gladLoadGL();
    fprintf(stderr, "    Graphics Language Loaded.\n    Graphics Language version: OpenGL %s\n", glGetString(GL_VERSION));
    
    std::cout << std::endl << std::endl << "Launching Game..." << std::endl;
    
    
    
    return EXIT_SUCCESS;
}
