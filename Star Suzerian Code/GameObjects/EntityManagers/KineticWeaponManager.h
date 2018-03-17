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
    virtual void processCollisions() override{;}
    virtual void handleInput(GLFWwindow * mWin) override {;}
    
    virtual void drawInstances() override {;}
    
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
