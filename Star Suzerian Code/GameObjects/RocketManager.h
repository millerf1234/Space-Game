//
//  RocketManager.h
//
//  Created by Forrest Miller on 2/26/18.
//

#ifndef RocketManager_h
#define RocketManager_h

//#include <stdio.h>
#include "Object.h"
#include "AACollisionBox.h"
#include "Generator.h"
#include "GameParameters.h"
#include "GameEntityManager.h"
#include "Instance.h"



class RocketManager : public GameEntityManager {
public:
    RocketManager() ;
    virtual ~RocketManager() override;
   
    void createNewInstance(const aiVector2D& pos, float red, float green, float blue);
    void createNewInstance(float x, float y, float z, float red, float green, float blue);
    
    virtual void doUpkeep() override;
    //virtual void handleInput() override;
    virtual void handleInput(GLFWwindow*) override;
    virtual void drawInstances() override;
    
    AACollisionBox colBox;
    
    Generator * generator;
    
    //aiVector3D getCenterWeaponLaunchpoint();
    //aiVector3D * getSideWeaponLaunchPoints();
    
    
protected:
    virtual void initializeFromTemplate() override;
private:
    void generateInitializationTemplate();
    //void processInput();
    
    
    //float max(const float&, const float&);
    
};

#endif /* RocketManager_h */
