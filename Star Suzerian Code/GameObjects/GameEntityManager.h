//
//  GameEntityManager.h
//
//  Created by Forrest Miller on 2/19/18.


#ifndef GameEntity_h
#define GameEntity_h

#include <stdio.h>
#include "GameParameters.h"
#include "Generator.h"

class GameEntityManager {
protected:
    //static std::shared_ptr<Generator *> sharedGenerator;
    Generator * generator;
    InitializationTemplate * initTemplate;
    specializationType specType;
    virtual void initializeFromTemplate() {;}
    
public:
    bool hasCollision;
    bool hasGenerator;
    bool requiresUserInput;
    bool requiresAIInput;
    bool hasSpecialization() {return (this->specType != specializationType::NOSPECIALIZATION);}
    
    virtual void doUpkeep() {;}
    virtual void handleInput() {;}
    virtual void drawInstances() {;}
    
//    virtual void deleteInstance(const int& instID);
//    virtual void deleteInstance(Instance *);
//    virtual void removeOldestInstance();
//    virtual void removeNewestInstance();
    
    GameEntityManager() {
        requiresUserInput = false;
        requiresAIInput = false;
        hasCollision = false;
        hasGenerator = false;
        generator = nullptr;
        initTemplate = nullptr;
        specType = NOSPECIALIZATION;
    }
    virtual ~GameEntityManager() {
        //Have children destruct what they construct
//        if (this->generator != nullptr) {
//            delete this->generator;
//            this->generator = nullptr;
//        }
//        if (this->initTemplate != nullptr) {
//            delete this->initTemplate;
//            this->initTemplate = nullptr;
//        }
    }
};

#endif /* GameEntity_h */
