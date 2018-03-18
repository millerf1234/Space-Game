// This is an object for the entire game. Basically once main is ready to launch the game,
// it calls a method (or methods) in this object that then takes over responibilty
// for handling the game.
//
// Note that WeaponOverseer is a class that manages the weapon classes which in turn
// manage their respective weapon types. As such, WeaponOverseer is not a GameEntityManager,
// and does not appear in the same array as all the other game entity managers.
// Currently all weaponTypeManagers are handled through WeaponOverseer, so they
// will not be a part of the GameEntity array.
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
#include "WeaponOverseer.h"
//#include "RocketManager.h"
#include "PlayerManager.h"
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
    unsigned long long frameUnpaused; //Needed to make pause/unpause operate correctly
    float counter;
    GLFWwindow * mWindow; //Pointer to the display that is being rendered too 
    Quaternion * xRot, * yRot, * zRot; //Pointers to global rotation quaternions
    //std::vector<Object *> activeGameObjects; //A vector of all active objects currently in the game (an active object may need to be rendered and may need to have collisions detected)
    
    Stage * stage;
    PlayerManager * playerManager;
    
    
   // BackGroundGenerator *background;
    
    int numberOfLevels; //Number of levels
    Stage ** levels; //An array of pointers to level
    
    
    //Idea: What if a game entity is like a Player or a Background or an enemy ai ship, i.e. something 'static' or 'permanent'.
    //Weapons will be a seperate thing because they aren't around that long and there could be a lot of them. They are still
    //treated as instances though... hmm
    
    std::vector<GameEntityManager *> gEntities; //Backgrounds, Players
    WeaponOverseer wepOverseer; //Is in charge of all aspects related to weaponry
    
    
    //Private Member Functions
    void draw();
    void processUserInput();
    void doGameLogic(); //Do upkeep for game objects (upkeep)
    void processInterEntityEvents(); //Handle inter-entity instance collision/creation/destruction
    
    
    void initializeWeaponsManager(); //

    void doZoomIn(unsigned long long framesToHold); //The most important function
};


#endif /* Game_h */

