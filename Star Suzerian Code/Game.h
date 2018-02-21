// This is an object for the entire game. Basically once main is ready to launch the game,
// it calls a method (or methods) in this object that then takes over responibilty
// for handling the game.
//
//  Game.h
//
//  Created by Forrest Miller on 2/12/18.
//

#ifndef Game_h
#define Game_h

#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>

#include "GLFW_Init.h" //Need this for the struct of monitor data passed in to constructor
#include "Quaternion.h" //Used for doing rotations
#include "Object.h" //Abstract base class for any in-game objects
//#include "Background.h"
#include "GameEntityManager.h"
#include "Stage.h" //I replaced Background.h with Stage.h
#include "GameParameters.h" //Get gameplay-affecting constants
static const char * INDENT = "        ";

class Game {
public:
    //Constructors
    Game() = delete; //Can't call default constructor
    Game(const Game &) = delete; //Can't call copy constructor
    Game(MonitorData &);
    
    //Destructor
    ~Game();
    
    void playIntroMovie(); //Implement this at some point

    void loadGameObjects(); //Combine this and loadShaders into 1?
    //void loadShaders(); //Combine this and loadGameObjects into 1?
    //void loadTextures();

    bool launch();

    //No copy assignment operator
    Game operator=(const Game&) = delete;
    
private:
    //Variables
    unsigned long long p1Score, p2Score, frameNumber;
    float counter;
    GLFWwindow * mWindow; //Pointer to the display that is being rendered too 
    Quaternion * xRot, * yRot, * zRot; //Pointers to global rotation quaternions
    //std::vector<Object *> activeGameObjects; //A vector of all active objects currently in the game (an active object may need to be rendered and may need to have collisions detected)
    
   // BackGroundGenerator *background;
    
    int numberOfLevels; //Number of levels
    Stage ** levels; //An array of pointers to level
    
    std::vector<GameEntityManager *> gEntities;
    bool shadersReady, texturesReady, objectGeneratorsReady;
    
    //Private Member Functions
    void draw();
    void processUserInput();
    void doGameLogic(); //Handle collisions and whatnot (upkeep)

    void doZoomIn(unsigned long long framesToHold); //The most important function
};


#endif /* Game_h */

