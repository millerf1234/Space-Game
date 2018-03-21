//  I wound up changing things again, so really now this is actually a class that
//  manages a player-controlled entity
//
//  Abstract Base Class for a user-controlled player (PSYCH!)
//  Actually just a class that can be controlled by a human player.
//  Player.h
//
//  Created by Forrest Miller on 2/13/18.
//

#ifndef Player_h
#define Player_h

#include <stdio.h>
//I use these next 2 to implement the pause functionality
#include <chrono> //c++11 to get nano time accuratly
#include <thread> //Used for thisThread::sleepFor() //(Bringing this whole thing is seems kinda overkill)

//#include "Object.h"
#include "Generator.h"
#include "GameParameters.h"
#include "GameEntityManager.h"
#include "Instance.h"
//#include "CollisionRectangle.h"

//#include "GLFW_Init.h"

//typedef struct PlayerInstance : public Instance {
//    float health;
//    float shields;
//    float energy;
//    float fuel;
//    int playerNumber;
//    int rocketCount;
//    int maxRockets;
//    
//    //Color information
//    float red;
//    float green;
//    float blue;
//    
//    PlayerInstance(int playerNumber) {
//        this->playerNumber = playerNumber;
//        health = STARTING_PLAYER_HEALTH;
//        shields = STARTING_PLAYER_SHIELDS;
//        energy = STARTING_PLAYER_ENERGY;
//        fuel = STARTING_PLAYER_FUEL;
//        rocketCount = STARTING_PLAYER_ROCKETS;
//        maxRockets = STARTING_PLAYER_ROCKET_COUNT_MAX;
//        red = green = blue = 0.75f; //Setting all 3 equal will give a shade of gray
//    }
//} PlayerInstance;

//This is a manager class that manages the active player instances
class PlayerManager : public GameEntityManager {
public:
    PlayerManager();
    virtual ~PlayerManager() override;
    //void addPlayer(struct controlBindings); //Add a public function like this?
    
    virtual void doUpkeep() override;
    virtual void processCollisions() override;
    //virtual void handleInput() override;
    virtual void handleInput(GLFWwindow*) override;
    virtual void drawInstances() override;
    
    int getNumberOfPlayerInstances() const {return (this->generator->getInstanceCount());}
    Instance** getPlayerInstances() const {return (this->generator->getArrayOfInstances());}
    
    //aiVector3D getCenterWeaponLaunchpoint();
    //aiVector3D * getSideWeaponLaunchPoints();
    
    
protected:
    virtual void initializeFromTemplate() override;
private:
    void generateInitializationTemplate();
    void processInput();
    
    float max(const float&, const float&);
    
    
};

////For reference:
//class Stage : public GameEntity {
//public:
//    Stage();
//    static int numberOfStages;
//    int stageNumber;
//
//    virtual ~Stage() override;
//    //Add some way to switch textures for background
//    virtual void doUpkeep() override ;
//    //virtual void handleInput() override; //Stage doesn't care about handling input
//    virtual void drawInstances() override;
//
//protected:
//    virtual void initializeFromTemplate() override;
//    void generateInitializationTemplate();
//};

//class GameEntity {
//protected:
//    Generator * generator;
//    InitializationTemplate * initTemplate;
//    specializationType specType;
//
//    virtual void initializeFromTemplate() {;}
//public:
//    bool hasCollision;
//    bool hasGenerator;
//    bool hasSpecialization() {return (this->specType != specializationType::NOSPECIALIZATION);}
//
//    virtual void doUpkeep() {;}
//    virtual void handleInput() {;}
//    virtual void drawInstances() {;}
//
//    GameEntity() {
//        hasCollision = false;
//        hasGenerator = false;
//        generator = nullptr;
//        initTemplate = nullptr;
//        specType = NOSPECIALIZATION;
//    }
//    virtual ~GameEntity() {




//Old class Player
//class Player /*: public Object */ { //Psych! Player ain't no object anymore!
//public:
//    float health;
//    int maxMissles;
//    static short playerNumber;
//
//
//
//    Generator * generator;
//
//
//
//
//
//private:
//    InitializationTemplate * stageInitFormat; //Contains information on how a stage should be constructed
//
//    void setStageInitFormat(int );
//
//
//};
//
//
////Set first player to player 1
//short Player::playerNumber = 1;
//
#endif /* Player_h */


