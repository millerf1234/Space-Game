//  Performs the necessary steps to get GLFW up and running on program startup.
//  Also does the steps to close the window once program has completed running.
//
//  GLFW_Init.cpp
//  
//  Created by Forrest Miller on 2/12/18.

#include "GLFW_Init.h"

//For reference: (This is data to be returned by initialize)
//typedef struct Monitors {
//    int numDetected, activeMonitorNum, width, height, refreshRate;
//    GLFWmonitor** monitorArray;
//    GLFWwindow* activeMonitor;
//    bool validContext;
//} Monitors;

//Do window setup routines and return a struct representing information on detected monitors
MonitorData GLFW_Init::initialize() {
    std::cout << "    Loading GLFW...";
    glfwInit();
    std::cout << "DONE!  GLFW version: " << glfwGetVersionString() << std::endl;
    std::cout << "    Setting Window Context Version: ";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->contextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->contextVersionMinor);
    std::cout << "set to version " << contextVersionMajor << "."
              << contextVersionMinor << std::endl;
    //I don't print out a message for this next one because window should pretty much always open in CORE compatibilty profile mode.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Set forward compatibility
    std::cout << "    Setting Forward Compatibility: ";
    if (this->forwardCompatible) {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        std::cout << "TRUE" << std::endl;
    }
    else {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
        std::cout << "FALSE" << std::endl;
    }
    //Set if window will be resizeable
    std::cout << "    Setting Window Resizeable: ";
    if (this->resizeable) {
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        std::cout << "TRUE" << std::endl;
    }
    else {
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        std::cout << "FALSE" << std::endl;
    }
    
    //glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE); //NOT SURE IF THIS IS NEEDED
    
    //Set Anti-Aliasing (Still not sure this makes any difference)
    std::cout << "    Window Global Anti-Aliasing set to: " << this->aaSamples << "x samples" << std::endl;
    glfwWindowHint(GL_SAMPLES, this->aaSamples);
    
    //Use VSync?
    if (this->vSyncInterval == 0) {
        std::cout << "    VSync set to: OFF" << std::endl;
        glfwSwapInterval(0); //Not sure if this line is needed because swap interval should be 0 by default (or at least so I believe)
    }
    else {
        glfwSwapInterval(this->vSyncInterval);
        std::cout << "    VSync set to: ON" << std::endl;
    }
    if (this->vSyncInterval != 0 && this->vSyncInterval != 1) { //if VSync interval is not set to 0 or 1
        std::cout << "WARNING! VSYNC INTERVAL IS SET TO A NON-STANDARD VALUE AND MAY RESULT IN UNDEFINED BEHAVIOR\n";
    }
    
    std::cout << "Window Initialization Complete" << std::endl;
    
    //Detect number of monitors connected:
    std::cout << std::endl << "Detecting Displays: " << std::endl;
    this->monitors = glfwGetMonitors(&(this->connectedDisplayCount));//Tell GLFW to look for all monitors connected currently.
    std::cout << "    Number of connected displays detected: " << this->connectedDisplayCount;
    std::cout << std::endl;
    
    //First try to make the window open on the specified display
    if (this->openFullScreen) {
    if (this->connectedDisplayCount >= this->defaultMonitor+1) { //If the desired default display is connected
        detectDisplayResolution(this->defaultMonitor, this->width, this->height, this->refreshRate);
        std::cout << "    Window will open on display: " << this->defaultMonitor+1 << std::endl;
        std::cout << "    Resolution of this Display: " << this->width << "x" << this->height << std::endl;
        std::cout << "    Display Refresh Rate: " << this->refreshRate << std::endl;
        
        std::cout << std::endl << "    Window Context is ready to open on this display" << std::endl;
        
        this->mWindow = glfwCreateWindow(this->width, this->height, NAME_OF_GAME,
                                         monitors[this->defaultMonitor], nullptr);
    }
    //If specified display is not connected, try opening on the first non-primary display detected
    else if (this->connectedDisplayCount >= 2) {
        detectDisplayResolution(1, this->width, this->height, this->refreshRate);
        std::cout << "    Window will open on display: " << 2 << std::endl;
        this->defaultMonitor = 1;
        std::cout << "    Resolution of this Display: " << this->width << "x" << this->height << std::endl;
        std::cout << "    Display Refresh Rate: " << this->refreshRate << std::endl;
        
        std::cout << std::endl << "    Window Context is ready to open on this display" << std::endl;
        
        this->mWindow = glfwCreateWindow(this->width, this->height, NAME_OF_GAME,
                                         monitors[1], nullptr);
    }
    //If all that fails, open on the primary display
    else {
        detectDisplayResolution(0, this->width, this->height, this->refreshRate);
        std::cout << "    Window will open on display: " << 1 << std::endl;
        this->defaultMonitor = 0;
        std::cout << "    Resolution of this Display: " << this->width << "x" << this->height << std::endl;
        std::cout << "    Display Refresh Rate: " << this->refreshRate << std::endl;
        
        std::cout << std::endl << "    Window Context is ready to open on this display" << std::endl;
        
        this->mWindow = glfwCreateWindow(this->width, this->height, NAME_OF_GAME,
                                         glfwGetPrimaryMonitor(), nullptr);
    }
    }
    else { //Open windowed
        std::cout << "    Window Context is ready to open in Windowed Mode" << std::endl;
        mWindow = glfwCreateWindow(1670, 960, NAME_OF_GAME, nullptr, nullptr); //Open as window
        this->defaultMonitor = 0;
    }
    
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        this->contextIsValid = false;
        //return nullptr;
    }
    else {
        glfwMakeContextCurrent(mWindow);
        //gladLoadGL(); //I call the gladLoadGL stuff in main, since it is not part of GLFW
        //fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
    }
    return generateDetectedMonitorsStruct();
}

//Creates a struct from the members of this class
MonitorData GLFW_Init::generateDetectedMonitorsStruct() {
    MonitorData displayDetectionResults; //Create struct to be returned
    //Set integers
    displayDetectionResults.numDetected = this->connectedDisplayCount;
    displayDetectionResults.activeMonitorNum = this->defaultMonitor;
    displayDetectionResults.width = this->width;
    displayDetectionResults.height = this->height;
    displayDetectionResults.refreshRate = this->refreshRate;
    //Set monitor pointers
    displayDetectionResults.monitorArray = this->monitors;
    displayDetectionResults.activeMonitor = this->mWindow;
    
    displayDetectionResults.validContext = this->contextIsValid;
    
    return displayDetectionResults;
}

//Detects the resolution of the active display
void GLFW_Init::detectDisplayResolution(int displayNum, int& width, int& height, int& refreshRate) {
    const GLFWvidmode * mode = nullptr;
    if (displayNum == 0) {
        mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    }
    else {
        mode = glfwGetVideoMode(this->monitors[displayNum]);
    }
    //Make sure mode isn't nullptr for some reason
    if (mode != nullptr) {
        width = mode->width;
        height = mode->height;
        refreshRate = mode->refreshRate;
    }
    else {
        std::cout << "\nOh No! GLFW encountered an error while communicating with your display!" << std::endl << "Error is due to: UNABLE TO RETRIEVE MONITOR VIDEO MODE INFORMATION. TRY A DIFFERENT MONITOR" << std::endl;
        this->contextIsValid = false;
    }
}

void GLFW_Init::terminate() {
    glfwTerminate(); //Terminating is quite a bit easier than setting up, as you can see
}
