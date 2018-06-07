//
//  KineticWeaponManager.cpp
//
//  Created by Forrest Miller on 3/16/18.
//

#include "KineticWeaponManager.h"

KineticWeaponManager::KineticWeaponManager() : GameEntityManager() {
    //GameEntityManager's constructer will handle setting everything to null
    
    this->initTemplate = new InitializationTemplate;
    this->generator = new Generator();
    this->hasGenerator = true;
    this->hasCollision = true;
    this->requiresUserInput = false;
    this->requiresAIInput = false;
    this->specType = specializationType::WEAPON;
    this->generator->specialization = specializationType::WEAPON;
    
    this->generateInitializationTemplate();
    
    //Configure the generator based off the initialization Template()
    this->initializeFromTemplate();
    
    //from stage:
    //GameEntity's constructor will set everything to 0 or Nullptr
    //    this->initTemplate = new InitializationTemplate;
    //    this->generator = new Generator();
    //    this->hasGenerator = true;
    //    this->hasCollision = false;
    //    this->stageNumber = ++numberOfStages;
    //
    //    //Have the stage initialize it's generator from it's template
    //    generateInitializationTemplate(); //Generate the initialization template for Stage
    //    initializeFromTemplate(); //Initialize Generator from the template
    
    
    //from Player:
    //    this->initTemplate = new InitializationTemplate;
    //    this->generator = new Generator;
    //    this->hasGenerator = true;
    // /Users/forrestmiller/Desktop/SpaceGameBuildTest/Star Suzerian 2018-03-25 13-00-50/Products/usr/local/bin/Star Suzerian   this->hasCollision = true;
    //    this->requiresUserInput = true; //Player controlled characters require user input
    //    //Set specialization type
    //    this->specType = specializationType::PLAYER;
    //    //Set-up the initialization template
    //    this->generateInitializationTemplate();
    //    //    this->generator->setSpecialization(PLAYER);
    //    //this->generator->specialization = specializationType::PLAYER;
    //    //Initialize the generator from the initialization template
    //    this->initializeFromTemplate(); //generator->setSpecialization now called inside this function
    
}

KineticWeaponManager::~KineticWeaponManager() {
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG:KineticWeaponsManager destructor called!";
    }
    if (this->initTemplate != nullptr) {
        if (this->initTemplate->hasVertsAlreadyLoaded) {
            //Then delete heap data that was used
            if (this->initTemplate->vertices != nullptr) {
                delete [] initTemplate->vertices;
                initTemplate->vertices = nullptr;
            }
            if (initTemplate->elements != nullptr) {
                delete [] initTemplate->elements;
                initTemplate->elements = nullptr;
            }
        }
        delete this->initTemplate;
        this->initTemplate = nullptr;
    }
    if (this->generator != nullptr) {
        delete this->generator;
        this->generator = nullptr;
    }
    
   
}
void KineticWeaponManager::generateInitializationTemplate() {
    //Set up the initializationTemplate for kinetic
    initTemplate->hasTexture = false;
    initTemplate->vertAttribName = "position";
    initTemplate->vert3 = true;
    
    initTemplate->vertShaderPath = KINETIC_VERT;
    initTemplate->hasVert = true;
    initTemplate->fragShaderPath = KINETIC_FRAG;
    initTemplate->hasFrag = true;
    
    
    initTemplate->typeDataRequired = WEAPON;
    
    //Model data
    initTemplate->hasVertsAlreadyLoaded = true;
    initTemplate->numVerts = KINETIC_PROJECTILE_VERSION0_VERTS_COUNT; //Don't change until collision box is reimplemented for each different Kinetic Projectile Shape
    //initTemplate->numVerts = KINETIC_PROJECTILE_VERSION6_VERTSSTART + KINETIC_PROJECTILE_VERSION6_VERTS_COUNT;
    initTemplate->vertices = new GLfloat[initTemplate->numVerts];
    //I actually changed it to do drawArrays only now, but haven't gotten around yet to removing the old element set-up code
    //This should be done with draw arrays, but going to use an element buffer anyways
    initTemplate->numElems = (KINETIC_PROJECTILE_VERSION0_VERTS_COUNT * 2) / 3;
    initTemplate->elements = new GLuint[initTemplate->numElems];
    
    //Need to align the pyramid to shoot in the correct direction (and so collision box forms correctly)
    Quaternion alignment(1.0f, 0.0f, 0.0f, PI / 2.0f);
    
    int halfPyrimidVertCount = initTemplate->numVerts / 2;
    aiVector3D vecsToRotate[halfPyrimidVertCount / 3];
    int counter = 0;
    
    
    for (int i = 0; i < halfPyrimidVertCount / 3; i++){ //was i++
        vecsToRotate[i] = aiVector3D(KINETIC_PROJECTILE_VERSION0_VERTS[counter], KINETIC_PROJECTILE_VERSION0_VERTS[counter+1], KINETIC_PROJECTILE_VERSION0_VERTS[counter + 2]);
        
        //Do the rotation on the vector as well to align the pyramid
        vecsToRotate[i] = alignment.computeRotation(vecsToRotate[i]);
        //Copy the rotated vector into the template's vertice array
        initTemplate->vertices[counter] = vecsToRotate[i].x;
        initTemplate->vertices[counter + 1] = vecsToRotate[i].y;
        initTemplate->vertices[counter + 2] = vecsToRotate[i].z;
        counter += 3;
    }
    //If not rotating Pyramid, then just copy the data straight over
//    for (int i = 0; i < halfPyrimidVertCount / 2; i++) {
//        initTemplate->vertices[i] = KINETIC_PROJECTILE_VERTS[i];
//    }
    //Flip to draw the other half of the pyrimid
    for (int i = halfPyrimidVertCount; i < initTemplate->numVerts; i++) {
        if (i % 3 == 2) {
            initTemplate->vertices[i] = KINETIC_PROJECTILE_VERSION0_VERTS[i - halfPyrimidVertCount] * -1.0f;
        }
        else {
            initTemplate->vertices[i] = KINETIC_PROJECTILE_VERSION0_VERTS[i - halfPyrimidVertCount];
        }
    }
    
    //Scale the PyrimidVerts to be smaller by a factor
    float pyramidShrinkFactor = 1.0 / PROJECTILE_SIZE;
    for (int i = 0; i < initTemplate->numVerts; i++) {
        initTemplate->vertices[i] = pyramidShrinkFactor * initTemplate->vertices[i];
    }
    
    //Fill in the element buffer as well
    for (int i = 0; i < initTemplate->numElems; i++) {
        initTemplate->elements[i] = (GLuint)i;
    }
}

void KineticWeaponManager::initializeFromTemplate() {
    generator->initializeFromTemplate(*initTemplate);
    if (generator->readyToGenerate()) {
        this->isReady = true; //I think I can set this here safely
    }
}

void KineticWeaponManager::spawnNewKineticInstance(WeaponTracker * wepTracker) {
    //if (wepTracker->getframesSinceKineticWasLastFired() == 0) //Do this when I have more time...
    int newInstanceIndex = this->generator->getInstanceCount();
    this->generator->generateSingle(); //Have generator create a new instance
    Instance ** insts = this->generator->getArrayOfInstances();
    
    //Do special configuration to set up the newly created KineticInstance
    Kinetic * newKinInst = static_cast<Kinetic*>(insts[newInstanceIndex]); //Since newInstanceIndex was set before generating new, it will be set to the array position of the newly created instance
    
    //newKinInst->mass = 0.01f; //Give it a tiny mass (mass doesn't actually do anything)
    newKinInst->zoom = GAME_SCALE; //I have to keep zoom the same for everything for position/velocity coordinates to line up
    //newKinInst->zoom = PLAYER_SIZE * 2.0f;
    //Need to figure out where to set colBox midpoint. Once midpoint and a velocity direction set, it
    //shouldn't need to be updated again...
    newKinInst->colBox->setScale( (1.0f / (newKinInst->zoom)));
    
    
    //Set up the rotation order on the collision box

    
    //Set up the rest of the new Kinetic Instance from data from the wepTracker
    newKinInst->offsetForLaunchpoint = wepTracker->getNextSpawnPoint();
    
    Quaternion earlyZRot(0.0f, 0.0f, 1.0f, wepTracker->getEarlyThetaZ());
    Quaternion zRot(0.0f, 0.0f, 1.0f, wepTracker->getThetaZ());
    Quaternion xRot(1.0f, 0.0f, 0.0f, wepTracker->getThetaX());
    Quaternion yRot(0.0f, 1.0f, 0.0f, 0.0f);
    newKinInst->colBox->addToRotationOrder(earlyZRot);
    newKinInst->colBox->addToRotationOrder(xRot);
    newKinInst->colBox->addToRotationOrder(zRot);
    newKinInst->colBox->addToRotationOrder(yRot);
    
    //Need to get an accurate midpoint for the kinetic projectile's midpoint...
    aiVector3D worldSpaceLaunchPointOffset = earlyZRot.computeRotation(newKinInst->offsetForLaunchpoint);
    worldSpaceLaunchPointOffset = xRot.computeRotation(worldSpaceLaunchPointOffset);
    worldSpaceLaunchPointOffset = zRot.computeRotation(worldSpaceLaunchPointOffset);
    
    aiVector2D worldSpaceLaunchPointOffset2D = aiVector2D(worldSpaceLaunchPointOffset.x, worldSpaceLaunchPointOffset.y);
    
    newKinInst->collisionBoxMidpoint = wepTracker->getPosition() + worldSpaceLaunchPointOffset2D; //Player's midpoint + weapon launch offset
    newKinInst->colBox->setMidpointTo(newKinInst->collisionBoxMidpoint);
    newKinInst->position = aiVector3D(newKinInst->collisionBoxMidpoint.x, newKinInst->collisionBoxMidpoint.y, 0.0f);
    
    //newKinInst->colBox->setScale(0.8);
    
    //Set forward for the kinetic projectile
    aiVector2D forward = wepTracker->getForwardDirection();
    newKinInst->forward = new aiVector3D(forward.x, -forward.y, 0.0f);
    
    //Set velocity for the projectile (note velocity is calculated based off ship's direction and ships velocity
    newKinInst->velocity = aiVector2D(forward.x, -forward.y);
    if (newKinInst->velocity.Length() != 0.0f) {
        newKinInst->velocity = newKinInst->velocity.Normalize();
    }
    else { //Velocity was assigned the 0 vector, so must
        newKinInst->velocity = aiVector2D(0.0f, 1.0f); //Give it a direction
    }
    
    newKinInst->velocity += wepTracker->getVelocity();
    ///We can check to see if ship is going backwards and align shots to shoot straight backwards
    if (ALIGN_FIRING_IF_FACING_BACKWARDS_WHILE_TRAVELING) {
        std::cout << "\nWARNING! Turning on ALIGN_FIRING_IF_FACING_BACKWARDS_WHILE_TRAVELING doesn't work correctly and may cause weird problems!\n";
        float dotProductOfForwardFacingVectorAndShipVelocity = (forward.x * newKinInst->velocity.x + forward.y * newKinInst->velocity.y);
        //if ( (dotProductOfForwardFacingVectorAndShipVelocity < 0.0f) && //This didn't work
        if ( ( (MathFunc::sgn(forward.x) != MathFunc::sgn(wepTracker->getVelocity().x) ) &&
               (MathFunc::sgn(forward.y) != MathFunc::sgn(wepTracker->getVelocity().y) )    ) &&
            (abs(dotProductOfForwardFacingVectorAndShipVelocity) / (newKinInst->velocity.Length()) ) > 0.85f) {
            newKinInst->velocity = forward * -1.0f;
            
        }
    }
    
    //* KINETIC_SPEED_FACTOR * PLAYER_MOVEMENT_MAX_SPEED;
    newKinInst->velocity *= KINETIC_SPEED_FACTOR * PLAYER_MOVEMENT_MAX_SPEED;
    newKinInst->thetaX = wepTracker->getThetaX();
    newKinInst->thetaY = 0.0f;
    newKinInst->thetaZ = wepTracker->getThetaZ();
    newKinInst->earlyThetaZ = wepTracker->getEarlyThetaZ();
    
    wepTracker->resetWeaponsFired(); //Reset weapons fired after a weapon was fired 
}

void KineticWeaponManager::doUpkeep() {
    this->generator->doUpkeep(); //Handle object upkeep
    if (this->generator->getInstanceCount() > 0) {
        Instance ** insts = this->generator->getArrayOfInstances();
        for (int i = 0; i < this->generator->getInstanceCount(); ++i) {
            Kinetic * k = static_cast<Kinetic*> (insts[i]);
            
            //Debug code 
            if (k->colBox == nullptr) {
                std::cout << "Something is wrong!\n\n";
                return;
            }
            
            //Put some changing rotation on the projectiles
            k->thetaY += PI / 24.0f;
            k->thetaZ += PI / 24.0f;
            k->thetaX -= PI / 24.0f;
            if ( ((int) (floor(k->timeAlive * 100.0f))) % 5 == 3) {
                k->thetaZ -= PI / 33.0f;
                k->thetaX -= PI / 31.0f;
            }
            
            //Update the rotations
            //k->colBox->changeRotationAt(0, k->earlyThetaZ);
            //k->colBox->changeRotationAt(1, -(k->thetaX));
            //k->colBox->changeRotationAt(2, k->thetaZ);
            //k->colBox->changeRotationAt(3, k->thetaY);
            
            //Translate the kinetic projectile by its velocity
            k->colBox->translate(k->velocity);
            //see if the kinetic projectile went off the side of the map
            aiVector2D kMidpoint = k->colBox->getMidpoint();
            if (kMidpoint.x > XLIMIT + 5.0f || kMidpoint.x < -XLIMIT - 5.0f || kMidpoint.y > YLIMIT + 5.0f || kMidpoint.y < -YLIMIT - 5.0f) {
                this->generator->removeInstance(insts[i]); //Remove the instance
                //This caused me many problems, but since I have deleted the instance here I need to shift
                //i back by one since all of the other instances will have shifted too
                i--;
                //insts now points to invalid memory unfortunatly... So need to update all insts
                insts = this->generator->getArrayOfInstances(); //Update insts to the new instants list
            }
            
        }
    }
    processCollisions(); //
}

void KineticWeaponManager::deleteFlaggedWeaponInstances() {
    if (this->generator->getInstanceCount() > 0) {
        Instance ** arrayOfInstances = this->generator->getArrayOfInstances();
        for (int i = 0; i < this->generator->getInstanceCount(); ++i) {
            Kinetic * k = static_cast<Kinetic*> (arrayOfInstances[i]);
            if (k->shouldBeDestroyed) {
                this->generator->removeInstance(arrayOfInstances[i]);
                i--; //The instance was removed, so account for its removal in the loop
                //The array of instances was changed by generator, so need to get it again
                arrayOfInstances = this->generator->getArrayOfInstances();
            }
        }
    }
}
