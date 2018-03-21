//
//  KineticWeaponManager.h
//
//  Created by Forrest Miller on 3/16/18.
//

#ifndef KineticWeaponManager_h
#define KineticWeaponManager_h

#include <iostream>
//#include "WeaponManager.h"
#include "Kinetic.h"
#include "GameEntityManager.h"
#include "GameParameters.h"

class KineticWeaponManager : public GameEntityManager {
public:
    KineticWeaponManager();
    virtual ~KineticWeaponManager() override;
    
    virtual void doUpkeep() override;
    virtual void processCollisions() override{std::cout << "\nKinetic collisions not yet being processed...\n";}
    virtual void handleInput(GLFWwindow * mWin) override {;}
    
    virtual void drawInstances() override {
        if (this->generator->getInstanceCount() > 0) { //Check to see if there is anything to draw
            std::cout << "\nIn Kinetic Weapons Manager doing drawInstances. Current Number of Kinetic Instances: ";
            std::cout << this->generator->getInstanceCount() << std::endl;
            std::cout << "Position of first kinetic instance: ";
            Instance ** tempForDebug = generator->getArrayOfInstances();
            aiVector3D tempPos = tempForDebug[0]->position;
            std::cout << tempPos.x << ", " << tempPos.y << ", " << tempPos.z << std::endl;
            
            aiVector2D tempColBoxPos = tempForDebug[0]->colBox->getMidpoint();
            std::cout << "Position of this inst's colBox: " << tempColBoxPos.x << ", " << tempColBoxPos.y << std::endl;
            
            //Draw instances
            this->generator->drawInstances();
            
        }
    }
    
    void spawnNewKineticInstance(WeaponTracker * wepTracker);
    
    bool getIsReady() const {return isReady;}
    
protected:
    virtual void initializeFromTemplate() override;
private:
    void generateInitializationTemplate();
    void processInput();
    
    bool isReady;
    float max(const float&, const float&);
    
};


#endif /* KineticWeaponManager_h */
