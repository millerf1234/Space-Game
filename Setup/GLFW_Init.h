//  See .cpp file for more info
//
//  GLFW_Init.h
//
//  Created by Forrest Miller on 2/12/18.
//

#ifndef GLFW_Init_h
#define GLFW_Init_h

#include "glfw3.h"
#include "GameParameters.h"
#include <iostream>

//See line 2623 in glfw3.h to see how to make window do alt+tab?

typedef struct MonitorData {
    int numDetected, activeMonitorNum, width, height, refreshRate;
    GLFWmonitor** monitorArray;
    GLFWwindow* activeMonitor;
    bool validContext;
} MonitorData;

class GLFW_Init {
private:
    int connectedDisplayCount; //Number of detected connected displays
    GLFWmonitor** monitors;  //Array of pointers to connected displays
    GLFWwindow* mWindow; //The display to be rendering to (might add ability to change displays in the future
    bool resizeable, forwardCompatible; //Configuration option booleans
    int contextVersionMajor, contextVersionMinor, aaSamples, vSyncInterval; //Configuration option integers
    bool contextIsValid; //Will become false if an initialization step fails
    bool openFullScreen; //
    int defaultMonitor;
    int width, height, refreshRate;
    
    MonitorData generateDetectedMonitorsStruct();  //Private function to be called to generate the return struct of monitor data at the end of initialize().
    
public:
    GLFW_Init() {
        width = height = refreshRate = 0;
        connectedDisplayCount = -77; //Error code for in case detecting displays fails completly (I think GLFW's error code could be -1)
        monitors = nullptr;
        mWindow = nullptr;
        resizeable = forwardCompatible = true;
        contextVersionMajor = DEFAULT_OPENGL_VERSION_MAJOR;
        contextVersionMinor = DEFAULT_OPENGL_VERSION_MINOR; //version 4.1 is the default version
        aaSamples = DEFAULT_AA_SAMPLES;
        if (USE_VSYNC) {
            vSyncInterval = 1; //Should only be 0 or 1;
        }
        else {
            vSyncInterval = 0;
        }
        openFullScreen = USE_FULLSCREEN;
        defaultMonitor = 1;
        contextIsValid = true;
    }
    
    GLFW_Init(int aaSamples, bool useVSync) {
        width = height = refreshRate = 0;
        connectedDisplayCount = -77; //Error code for in case detecting displays fails completly (I think GLFW's error code could be -1)
        monitors = nullptr;
        mWindow = nullptr;
        resizeable = forwardCompatible = true;
        contextVersionMajor = DEFAULT_OPENGL_VERSION_MAJOR;
        contextVersionMinor = DEFAULT_OPENGL_VERSION_MINOR; //version 4.1 is the default version
        this->aaSamples = aaSamples;
        if (useVSync) { vSyncInterval = 1; } //Should only be 0 or 1;
        else          { vSyncInterval = 0; }
        openFullScreen = USE_FULLSCREEN;
        defaultMonitor = 1;
        contextIsValid = true;
    }
    
    //Allow for a different context version to be specified at construction
    GLFW_Init(int contextVersionMajor, int contextVersionMinor) {
        width = height = refreshRate = 0;
        connectedDisplayCount = -77; //Error code for in case detecting displays fails completly (I think GLFW's error code could be -1)
        monitors = nullptr;
        mWindow = nullptr;
        resizeable = forwardCompatible = true;
        this->contextVersionMajor = contextVersionMajor;
        this->contextVersionMinor = contextVersionMinor;
        aaSamples = DEFAULT_AA_SAMPLES;
        if (USE_VSYNC) {
            vSyncInterval = 1; //Should only be 0 or 1;
        }
        else {
            vSyncInterval = 0;
        }
        openFullScreen = USE_FULLSCREEN;
        defaultMonitor = 1;
        contextIsValid = true;
    }
    
    void openWindowed() {this->openFullScreen = false;}
    void setDefaultMonitor(int monitorNumber) {this->defaultMonitor = monitorNumber;}
    
    MonitorData initialize(); //Sets up the window
    void terminate(); //Ends the window
    
    //Here is a public function for detecting information about a connected display (undefined behavior if displayNum is not a connected display)
    void detectDisplayResolution(int displayNum, int& width, int& height, int& refreshRate);
    
    //Ideas of stuff to add:
    //    Have it so if holding down 'w' key when program opens, program opens in windowed mode.
    //    Have it so if holding down 'v' key when program opens, program opens with/without vsync
    //    Have it so if holding down a number (with lowest as 1), window opens on that monitor (if a monitor corresponding to that number is detected).
    
};



#endif /* GLFW_Init_h */
