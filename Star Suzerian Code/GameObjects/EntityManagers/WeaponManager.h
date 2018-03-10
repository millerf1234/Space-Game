//
//  WeaponManager.h
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

class WeaponManager : public GameEntityManager {
public:
    WeaponManager();
    virtual ~WeaponManager() override {;} //Give empty function body for now...
    
    virtual void doUpkeep() override {;}
    virtual void processCollisions() override{;}
    virtual void handleInput(GLFWwindow * mWin) override {;}
    
    virtual void drawInstances() override {;}
    
private:
    
};


#endif /* WeaponManager_h */


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
