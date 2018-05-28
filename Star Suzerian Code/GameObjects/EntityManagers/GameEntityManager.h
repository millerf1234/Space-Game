//
//  GameEntityManager.h
//
//  Created by Forrest Miller on 2/19/18.


#ifndef GameEntity_h
#define GameEntity_h

#include <iostream>
#include "GameParameters.h"
#include "Generator.h"
#include "GLFW_Init.h"
#include "Impact2D.h"

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
    
    specializationType getSpecType() const {return this->specType;} //I never use this function
    
    void ageObjects();
    virtual void doUpkeep() {;}
    virtual void handleInput() {;}
    virtual void handleInput(GLFWwindow* activeMonitor) {;}
    virtual void processCollisions() {;}
    virtual void drawInstances() {;}
    
    Instance** getInstances() {return this->generator->getArrayOfInstances();}
    int getNumberOfInstances() {return this->generator->getInstanceCount();}
    
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
        if (PRINT_DESTRUCTOR_CALLS) {
            std::cout << "\nDEBUG:GameEntityManager destructor called!\n";
        }
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
