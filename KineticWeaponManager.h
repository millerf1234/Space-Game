//
//  KineticWeaponManager.h
//
//  Created by Forrest Miller on 3/16/18.
//

#ifndef KineticWeaponManager_h
#define KineticWeaponManager_h

#include <iostream>
#include <vector>
//#include "WeaponManager.h"
#include "Kinetic.h"
#include "GameEntityManager.h"
#include "GameParameters.h"
#include "MathFunctions.h" //For randomInRange()


class KineticWeaponManager : public GameEntityManager {
public:
    KineticWeaponManager();
    virtual ~KineticWeaponManager() override;
    
    virtual void doUpkeep() override;
    virtual void processCollisions() override{/*std::cout << "\nKinetic collisions not yet being processed...\n";*/}
    virtual void handleInput(GLFWwindow * mWin) override {;}
    
    virtual void drawInstances() override {
        if (this->generator->getInstanceCount() > 0) { //Check to see if there is anything to draw
            //std::cout << "\nIn Kinetic Weapons Manager doing drawInstances. Current Number of Kinetic Instances: ";
            //std::cout << this->generator->getInstanceCount() << std::endl;
            //std::cout << "Position of first kinetic instance: ";
            Instance ** tempForDebug = generator->getArrayOfInstances();
            aiVector3D tempPos = tempForDebug[0]->position;
            //std::cout << tempPos.x << ", " << tempPos.y << ", " << tempPos.z << std::endl;
            
            //aiVector2D tempColBoxPos = tempForDebug[0]->colBox->getMidpoint();
            //std::cout << "Position of this inst's colBox: " << tempColBoxPos.x << ", " << tempColBoxPos.y << std::endl;
            
            //Draw instances
            this->generator->drawInstances();
            
        }
    }
    
    int getNumberOfActiveInstances() const {return this->generator->getInstanceCount();}
    Instance ** getActiveInstances() {
        if (this->generator->getInstanceCount() == 0) {
            //Print a warning message
            std::cout << "\nDEBUG::getActiveInstances() called on KineticWeaponManager but this manager is not managing any active instances!\nNullptr will be returned, code should check to make sure there are active instances before calling this function!\n";
            return nullptr;
        }
        else {
            return this->generator->getArrayOfInstances();
        }
    }
    
    void spawnNewKineticInstance(WeaponTracker * wepTracker);
    
    void deleteFlaggedWeaponInstances();
    
    bool getIsReady() const {return isReady;}
    
protected:
    virtual void initializeFromTemplate() override;
private:
    void generateInitializationTemplate();
    void processInput();
    
    bool isReady;
    float max(const float&, const float&);
    
    std::unique_ptr<SoundEffect> kineticFireSound;
    std::vector<std::unique_ptr<AudioSource>> kineticFireSoundSources;
    //std::unique_ptr<AudioSource> kineticFireSoundSource;
    
    unsigned long long framesSinceSoundLastPlayed;
};


#endif /* KineticWeaponManager_h */
