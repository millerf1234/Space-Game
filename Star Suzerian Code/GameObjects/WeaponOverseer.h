//
//  WeaponManager.h
//
//  Even though it is bad practice, this class is intended to be used as a singleton
//  to handle holding all the weapon instance data.
//
//  Created by Forrest Miller on 3/8/18.
//

#ifndef WeaponManager_h
#define WeaponManager_h

#include <iostream>
#include "GameEntityManager.h"
#include "CollisionBox.h"
#include "Generator.h"
#include "Quaternion.h"
#include "GameParameters.h"
#include "KineticWeaponManager.h"

class WeaponOverseer {
public:
    WeaponOverseer();
    ~WeaponOverseer();
    
    void initializeWeapons();
    bool getIsReady() const {return this->isReady;}
    
    void ageWeaponInstances();
    void doUpkeep();
    void drawInstances() {
        kineticWepManager->drawInstances();
        //otherWepManagers->drawInsances();
    }
    
    void processWeaponTrackers();
    
    void getAllActiveWeaponInstances(std::vector<WeaponInstance *> & activeWepInsts) {
        //Get all the active Kinetic weapon instances
        int numActiveInstances = kineticWepManager->getNumberOfActiveInstances();
        if (numActiveInstances > 0) {
            Instance** insts = kineticWepManager->getActiveInstances();
            //Cast each instance to a weapon instance, and then place it inside the vector
            for (int i = 0; i < numActiveInstances; i++) {
                WeaponInstance * wepInst = static_cast<WeaponInstance *>(insts[i]);
                activeWepInsts.push_back(wepInst);
            }
        }
        
        //Get additional weapon instance types, and place them into the vector
    }
    
    void deleteFlaggedWeaponInstances() {
        kineticWepManager->deleteFlaggedWeaponInstances();
        //Other weapon managers->deleteFlaggedWeaponInstances();
    }
    
    void generateAndAttachWeaponTrackerToInstance(Entity *);
    
private:
    bool isReady;
    
    //Weapon type managers
    KineticWeaponManager * kineticWepManager;
    
    WeaponTracker ** wepTrackers;
    int numWepTrackers;
};


//public:

//private




#endif /* WeaponManager_h */


////Reference:
////#include <iostream>
//#include "GameParameters.h"
//#include "Generator.h"
//#include "GLFW_Init.h"
//#include "Impact2D.h"
//
//class GameEntityManager {
//protected:
//    //static std::shared_ptr<Generator *> sharedGenerator;
//    Generator * generator;
//    InitializationTemplate * initTemplate;
//    specializationType specType;
//    virtual void initializeFromTemplate() {;}
//    
//public:
//    bool hasCollision;
//    bool hasGenerator;
//    bool requiresUserInput;
//    bool requiresAIInput;
//    bool hasSpecialization() {return (this->specType != specializationType::NOSPECIALIZATION);}
//    
//    void ageObjects();
//    virtual void doUpkeep() {;}
//    virtual void handleInput() {;}
//    virtual void handleInput(GLFWwindow* activeMonitor) {;}
//    virtual void processCollisions() {;}
//    virtual void drawInstances() {;}
//    
//    
//    
//    //    virtual void deleteInstance(const int& instID);
//    //    virtual void deleteInstance(Instance *);
//    //    virtual void removeOldestInstance();
//    //    virtual void removeNewestInstance();
//    
//    GameEntityManager() {
//        requiresUserInput = false;
//        requiresAIInput = false;
//        hasCollision = false;
//        hasGenerator = false;
//        generator = nullptr;
//        initTemplate = nullptr;
//        specType = NOSPECIALIZATION;
//    }
//    virtual ~GameEntityManager() {
//        //Have children destruct what they construct
//        //        if (this->generator != nullptr) {
//        //            delete this->generator;
//        //            this->generator = nullptr;
//        //        }
//        //        if (this->initTemplate != nullptr) {
//        //            delete this->initTemplate;
//        //            this->initTemplate = nullptr;
//        //        }
//    }
//};


/*
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
 
 
 aiVector3D getCenterWeaponLaunchpoint();
 aiVector3D * getSideWeaponLaunchPoints();
 
 
 protected:
 virtual void initializeFromTemplate() override;
 private:
 void generateInitializationTemplate();
 void processInput();
 
 float max(const float&, const float&);
 
 };
*/
