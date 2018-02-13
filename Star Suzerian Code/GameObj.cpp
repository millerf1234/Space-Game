//
//  GameObj.cpp
//  Star Suzerian
//
//  Created by Forrest Miller on 2/12/18.


#include "GameObj.h"

//Constructor
GameObj::GameObj(MonitorData &) {
    //Make the rotation quaternions (4th parameter is starting theta)
    std::cout << INDENT << "Generating Rotation Quaternions..." << std::endl;
    xRot = new Quaternion(1.0f, 0.0f, 0.0f, 0.0f); //rotation about x axis
    yRot = new Quaternion(0.0f, 1.0f, 0.0f, 0.0f); //rotation about y axis
    zRot = new Quaternion(0.0f, 0.0f, 1.0f, 0.0f); //rotation about z axis
    
}

//Destructor (basically does a 'delete x' for every 'new x' that was called in the constructor)
GameObj::~GameObj() { //std::cout << "\nDEBUG::GameObj destructor was called...\n";
    //Delete rotation Quaternions
    if (xRot != nullptr) {//Check to make sure they havn't already been deleted for some reason
        delete xRot;
        xRot = nullptr; //Set to nullptr to avoid dangling pointer
    }
    if (yRot != nullptr) {
        delete yRot;
        yRot = nullptr;
    }
    if (zRot != nullptr) {
        delete zRot;
        zRot = nullptr;
    }
}

void GameObj::playIntroMovie() {  }

void GameObj::loadGameObjects() {
    
}
void GameObj::loadShaders() {  }
void GameObj::loadTextures() {  }

bool GameObj::launch() {  return true;}

