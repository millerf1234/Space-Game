//
//  GameObj.h
//  Star Suzerian
//
//  Created by Forrest Miller on 2/12/18.
//

#ifndef GameObj_h
#define GameObj_h

#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>


#include "GLFW_Init.h" //Need this for the struct of monitor data passed in to constructor
#include "Quaternion.h" //Used for doing rotations

static const char * INDENT = "        ";

class GameObj {
public:
    //Constructors
    GameObj() = delete; //Can't call default constructor
    GameObj(const GameObj &) = delete; //Can't call copy constructor
    GameObj(MonitorData &);
    
    //Destructor
    ~GameObj();
    
    void playIntroMovie();

    void loadGameObjects(); //Combine this and loadShaders into 1?
    void loadShaders(); //Combine this and loadGameObjects into 1?
    void loadTextures();

    bool launch();

    //No copy assignment operator
    GameObj operator=(const GameObj&) = delete;
    
private:
    //Variables
    unsigned long long p1Score, p2Score, frameNumber;
    float counter;
    GLFWwindow * mWindow; //Pointer to the display that is being rendered too 
    Quaternion * xRot, * yRot, * zRot; //Pointers to global rotation quaternions
    
    //Private Member Functions
    void draw();
    void processUserInput();
    void doGameLogic(); //Handle collisions and whatnot

    void doZoomIn(unsigned long long secondsToHold); //The most important function
};


#endif /* GameObj_h */

