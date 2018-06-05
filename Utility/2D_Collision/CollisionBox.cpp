//
//  AACollisionBox.cpp
//
//  Created by Forrest Miller on 3/5/18.
//

#include "CollisionBox.h"

static const float STEP_SIZE = 0.01f; //Used for seperating two overlapping CollisionBoxes

static const int CUBOID_CORNERS = 8;
static const int BOX_CORNERS = 4;
static const int NOT_SET = -1;

// (Note to self) FUDGE_FACTOR could probably be renaimed FLOP_TOLERANCE or just TOLERANCE
//FUDGE_FACTOR is used to make there be a max/min x-y value in case there is a tie when generating 2D box
static const float FUDGE_FACTOR = 0.00001f; //FUDGE_FACTOR should be at least an order of magnitude less than the
                                            //smallest rotation change amount
//              NOTE THAT FUDGE FACTOR OF 0.000001f causes a crash because value is too small

//------------------------------------------------------------------------
//              CONSTRUCTORS
//------------------------------------------------------------------------
CollisionBox::CollisionBox() {
    initialize();
}

/*!
 Constructor that initializes the AACollisionBox straight from the model data.
 If model has no z component, just create an array with x and y coordinates as
 desired and set every third value to 0.0f.
 @remark  The expected format of the array Data is model data in the form:
 x0,y0,z0,x1,y1,z1,x2,y2,z2,...
 */
CollisionBox::CollisionBox(float * data, int dataPoints) {
    initialize();
    //Check to make sure model data is complete
    if (dataPoints % 3 != 0) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! Attempting to construct a Collision";
            std::cout << " Rectangle from\ndata that is not a multiple of 3!!";
            std::cout << std::endl;
        }
        return;
    }
    //Create arrays to break up the data points by axis
    float xDataPoints[dataPoints / 3];
    float yDataPoints[dataPoints / 3];
    float zDataPoints[dataPoints / 3];
    
    //Set all the values in these three arrays to 0
    for (int i = 0; i < dataPoints / 3; ++i) {
        xDataPoints[i] = 0.0f;
    }
    for (int j = 0; j < dataPoints / 3; ++j) {
        yDataPoints[j] = 0.0f;
    }
    for (int k = 0; k < dataPoints / 3; ++k) {
        zDataPoints[k] = 0.0f;
    }
    //potentially faster method (but this also could potentially be slower):
    //for (int i = 0; i < dataPoints / 2; ++i) {
    //    xDataPoints = yDataPoints = zDataPoints = 0.0f;
    //}
    
    //Load the model data into the arrays
    int counter = 0;
    for (int i = 0; i < dataPoints / 3; ++i) {
        xDataPoints[i] = data[counter];
        counter += 3;
    }
    counter = 1;
    for (int i = 0; i < dataPoints / 3; ++i) {
        yDataPoints[i] = data[counter];
        counter += 3;
    }
    counter = 2;
    for (int i = 0; i < dataPoints / 3; ++i) {
        zDataPoints[i] = data[counter];
        counter += 3;
    }
    
    //Get both the maximum positive and negative values along each axis
    float maxPosX, maxNegX, maxPosY, maxNegY, maxPosZ, maxNegZ;
    maxPosX = maxNegX = maxPosY = maxNegY = maxPosZ = maxNegZ = 0.0f; //Initialize all 6 to 0.
    
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, dataPoints / 3);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, dataPoints / 3);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, dataPoints / 3);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, dataPoints / 3);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, dataPoints / 3);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, dataPoints / 3);
    
    
    ///Figure out which direction along each basis axis will be major and which will be minor
    
    ///Along the x axis:
    if (maxPosX >= abs(maxNegX)) {
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
    }
    else {
        //use negative as major
        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxNegX;
        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxPosX;
    }
//    
//    //Along the x axis:
//    if (maxPosX == abs(maxNegX)) {
//        //use positive as major
//        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
//        originalMajorsFromModel.x = maxPosX;
//        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
//        originalMinorsFromModel.x = maxNegX;
//    }
//    else if (maxPosX > abs(maxNegX)) { //Else if positive x values have larger max then neg x values
//        //use positive as major
//        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
//        originalMajorsFromModel.x = maxPosX;
//        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
//        originalMinorsFromModel.x = maxNegX;
//    }
//    else { //else negative x must have contained overall larger values
//        //use negative as major
//        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
//        originalMajorsFromModel.x = maxNegX;
//        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
//        originalMinorsFromModel.x = maxPosX;
//    }
    
    //Along the y axis:
    if (maxPosY == abs(maxNegY)) {
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
    }
    else if (maxPosY > abs(maxNegY)) { //Else if positive Y values have larger max then negative Y values
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
    }
    else { //else negative y must have contained overall larger values
        //use negative as major
        yAxisMajor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMajorsFromModel.y = maxNegY;
        yAxisMinor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMinorsFromModel.y = maxPosY;
    }
    
    //Along the z axis:
    if (maxPosZ == abs(maxNegZ)) { //If the largest distance is equal in both directions
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
    }
    else if (maxPosZ > abs(maxNegZ)) { //Else if positive Z values have larger max then negative Z values
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
    }
    else { //else negative z must have contained overall larger values
        //use negative as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMajorsFromModel.z = maxNegZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMinorsFromModel.z = maxPosZ;
    }
    this->hasModelData = true;
    
    buildCornerAdjacencyList();
    doRotationsAndRecalculate(); //This will set up the box based off the axis data
}

CollisionBox::CollisionBox(aiVector3D * data, int numberOfVectors) {
    initialize();
    if (numberOfVectors <= 0) {
        if (printDebugWarnings || printDebugMessages) {
            std::cout << "DEBUG::Warning! AACollisionBox attempted to be const";
            std::cout << "ructed from a vector count of " << numberOfVectors;
            std::cout << "\nIs this intentional? No box will be constructed...";
            std::cout << std::endl;
        }
    }
    //Create arrays to break up the data points by axis
    float xDataPoints[numberOfVectors];
    float yDataPoints[numberOfVectors];
    float zDataPoints[numberOfVectors];
    
    //Set all the values in these three arrays to 0
    for (int i = 0; i < numberOfVectors; ++i) {
        xDataPoints[i] = data[i].x;
        yDataPoints[i] = data[i].y;
        zDataPoints[i] = data[i].z;
    }
    
    //Get both the maximum positive and negative values along each axis
    float maxPosX, maxNegX, maxPosY, maxNegY, maxPosZ, maxNegZ;
    maxPosX = maxNegX = maxPosY = maxNegY = maxPosZ = maxNegZ = 0.0f; //Initialize all 6 to 0.
    
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, numberOfVectors);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, numberOfVectors);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, numberOfVectors);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, numberOfVectors);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, numberOfVectors);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, numberOfVectors);
    
    //Figure out which direction along each basis axis will be major and which will be minor
    //Along the x axis:
    if (maxPosX == abs(maxNegX)) {
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
        xAxisSymmetry = true;
    }
    else if (maxPosX > abs(maxNegX)) { //Else if positive x values have larger max then neg x values
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
        xAxisSymmetry = false;
    }
    else { //else negative x must have contained overall larger values
        //use negative as major
        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxNegX;
        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxPosX;
        xAxisSymmetry = false;
    }
    
    //Along the y axis:
    if (maxPosY == abs(maxNegY)) {
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
        yAxisSymmetry = true;
    }
    else if (maxPosY > abs(maxNegY)) { //Else if positive Y values have larger max then negative Y values
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
        yAxisSymmetry = false;
    }
    else { //else negative y must have contained overall larger values
        //use negative as major
        yAxisMajor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMajorsFromModel.y = maxNegY;
        yAxisMinor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMinorsFromModel.y = maxPosY;
        yAxisSymmetry = false;
    }
    
    //Along the z axis:
    if (maxPosZ == abs(maxNegZ)) { //If the largest distance is equal in both directions
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
        zAxisSymmetry = true;
    }
    else if (maxPosZ > abs(maxNegZ)) { //Else if positive Z values have larger max then negative Z values
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
        zAxisSymmetry = false;
    }
    else { //else negative z must have contained overall larger values
        //use negative as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMajorsFromModel.z = maxNegZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMinorsFromModel.z = maxPosZ;
        zAxisSymmetry = false;
    }
    this->hasModelData = true;
    
    buildCornerAdjacencyList();
    doRotationsAndRecalculate(); //This will set up the box based off the axis data
}

//------------------------------------------------------------------------
//              DESTRUCTOR
//------------------------------------------------------------------------
CollisionBox::~CollisionBox() {
    if (PRINT_DESTRUCTOR_CALLS) {
        std::cout << "\nDEBUG:COLLISIONBOX Destructor called!";
    }
    
    //Clear up the dynamic array of rotation quaternions
    if (rotationOrder != nullptr) {
        for (int i = 0; i < rotationOrderSize; ++i) {
            if (rotationOrder[i] != nullptr) {
                delete rotationOrder[i];
                rotationOrder[i] = nullptr;
            }
        }
        delete [] rotationOrder;
    }
    
    //Delete the vectors of collision points
    if (collisionSamplePointsAll != nullptr) {
        delete collisionSamplePointsAll;
        collisionSamplePointsAll = nullptr;
    }
    if (collisionSamplePointsThis != nullptr) {
        delete collisionSamplePointsThis;
        collisionSamplePointsThis = nullptr;
    }
}

//------------------------------------------------------------------------
//              Set Model Data Function
//------------------------------------------------------------------------
void CollisionBox::setFromModelData(float * data, int dataPoints) {
    //Should I call initialize again? No, I shouldn't, so rotationOrder doesn't memory leak
    bool dataWasNotTheRightSize = false;
    if (dataPoints % 3 != 0) {
        if (printDebugWarnings || printDebugMessages) {
            std::cout << "DEBUG::WARNING! setFromModelData called on this ";
            std::cout << "AACollisionBox but the\ndata provided to this has";
            std::cout << " a size other than a multiple of 3!\n";
            std::cout << "AACollisionBox was not set from provided data!\n";
        }
        dataWasNotTheRightSize = true;
    }
    //Check to see if this object already has model data
    if (hasModelData) { //if object already had model data
        if (dataWasNotTheRightSize == false) { //and if newly provided data is divisble by 3
            if (printDebugWarnings || printDebugMessages) {
                std::cout << "\nDEBUG::ATTENTION! This AACollisionBox already";
                std::cout << " had model data set!\n";
                std::cout << "AACollsisionBox will now reconfigure itself from ";
                std::cout << "the new model data!\nPlease note that the rotation ";
                std::cout << "order will be cleared!" << std::endl;
            }
            //Clear the rotation order by
            //Clearing up the dynamic array of rotation quaternions
            if (rotationOrder != nullptr) {
                //for each rotation in the rotation order
                for (int i = 0; i < rotationOrderSize; ++i) {
                    //if the rotation exists, delete it
                    if (rotationOrder[i] != nullptr) {
                        delete rotationOrder[i];
                        rotationOrder[i] = nullptr;
                    }
                } //Delete the array of Quaternion pointers
                delete [] rotationOrder;
            }
        }
        else { //else there already was model data and new data is not the right size
            if (printDebugWarnings || printDebugMessages) {
                std::cout << "\nDEBUG::OOPS! This AACollisionBox already had";
                std::cout << " model data and the newly\nprovided data to it using";
                std::cout << " the function setFromModelData was not sized a\n";
                std::cout << "multiple of three!\nNew model data not set, so ";
                std::cout << "AACollisionBox will keep previous data\nit was";
                std::cout << " configured with!\n";
            }
            return; //Return without executing the rest of the function
        }
    } //If there wasn't already model data and provided data was not the right size
    else if (dataWasNotTheRightSize) {
        return; //A debug message for this case was already printed out when
        //the first check for dataWasNotTheRightSize was performed
    }
    
    //---------------------------------------------
    //Here is where the function actually starts configuring the new AACollisionBox
    //based off the provided data
    
    //Create arrays to break up the data points by axis
    float xDataPoints[dataPoints / 3];
    float yDataPoints[dataPoints / 3];
    float zDataPoints[dataPoints / 3];
    
    //Set all the values in these three arrays to 0
    for (int i = 0; i < dataPoints / 3; ++i) {
        xDataPoints[i] = 0.0f;
    }
    for (int j = 0; j < dataPoints / 3; ++j) {
        yDataPoints[j] = 0.0f;
    }
    for (int k = 0; k < dataPoints / 3; ++k) {
        zDataPoints[k] = 0.0f;
    }
    //potentially faster method (but this also could potentially be slower):
    //for (int i = 0; i < dataPoints / 2; ++i) {
    //    xDataPoints = yDataPoints = zDataPoints = 0.0f;
    //}
    
    //Load the model data into the arrays
    int counter = 0;
    for (int i = 0; i < dataPoints / 3; ++i) {
        xDataPoints[i] = data[counter];
        counter += 3;
    }
    counter = 1;
    for (int i = 0; i < dataPoints / 3; ++i) {
        yDataPoints[i] = data[counter];
        counter += 3;
    }
    counter = 2;
    for (int i = 0; i < dataPoints / 3; ++i) {
        zDataPoints[i] = data[counter];
        counter += 3;
    }
    
    //Get both the maximum positive and negative values along each axis
    float maxPosX, maxNegX, maxPosY, maxNegY, maxPosZ, maxNegZ;
    maxPosX = maxNegX = maxPosY = maxNegY = maxPosZ = maxNegZ = 0.0f; //Initialize all 6 to 0.
    
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, dataPoints / 3);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, dataPoints / 3);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, dataPoints / 3);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, dataPoints / 3);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, dataPoints / 3);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, dataPoints / 3);
    
    
    //Figure out which direction along each basis axis will be major and which will be minor
    //Along the x axis:
    if (maxPosX == abs(maxNegX)) {
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
        xAxisSymmetry = true;
    }
    else if (maxPosX > abs(maxNegX)) { //Else if positive x values have larger max then neg x values
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
        xAxisSymmetry = false;
    }
    else { //else negative x must have contained overall larger values
        //use negative as major
        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxNegX;
        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxPosX;
        xAxisSymmetry = false;
    }
    
    //Along the y axis:
    if (maxPosY == abs(maxNegY)) {
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
        yAxisSymmetry = true;
    }
    else if (maxPosY > abs(maxNegY)) { //Else if positive Y values have larger max then negative Y values
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
        yAxisSymmetry = false;
    }
    else { //else negative y must have contained overall larger values
        //use negative as major
        yAxisMajor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMajorsFromModel.y = maxNegY;
        yAxisMinor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMinorsFromModel.y = maxPosY;
        yAxisSymmetry = false;
    }
    
    //Along the z axis:
    if (maxPosZ == abs(maxNegZ)) { //If the largest distance is equal in both directions
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
        zAxisSymmetry = true;
    }
    else if (maxPosZ > abs(maxNegZ)) { //Else if positive Z values have larger max then negative Z values
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
        zAxisSymmetry = false;
    }
    else { //else negative z must have contained overall larger values
        //use negative as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMajorsFromModel.z = maxNegZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMinorsFromModel.z = maxPosZ;
        zAxisSymmetry = false;
    }
    this->hasModelData = true;
    
    buildCornerAdjacencyList(); //Build this list after model data is set
    doRotationsAndRecalculate(); //This will set up the box based off the axis data
}

//------------------------------------------------------------------------
//              Rotation Functions
//     (I have tentative plans to create a whole seperate class for rotation order...)
//------------------------------------------------------------------------
void CollisionBox::presetRotationOrderSize(int size) {
    //Allows for space to be allocated for all the rotations before adding them
    if (size <= 0 || size < this->rotationOrderSize) {
        if (printDebugMessages || printDebugWarnings) {
            if (size <= 0) {
                std::cout << "\nDEBUG::OOPS! The Rotation Queue for this CollisionRectangle was attemped to be\n";
                std::cout << "set to a size of " << size << ", which is invalid!" << std::endl;
            }
            else { //if (size < this->rotationOrderSize)
                std::cout << "\nDEBUG::OOPS! ATTEMPTING TO SET THE ROTATIONORDER ARRAY SIZE TO A SIZE SMALLER\n";
                std::cout << "THAN THE CURRENT ROTATIONORDER SIZE.\nATTEMPED TO SET TO SIZE: " << size;
                std::cout << "\nBUT THE ROTATIONORDER ARRAY IS ALREADY SIZE: " << this->rotationOrderSize << std::endl;
            }
        }
        return;
    }
    if (rotationOrder == nullptr) { //If a rotationOrder array hasn't been allocated yet
        rotationOrder = new Quaternion* [size]; //allocate 'size' amount of space
        for (int i = 0; i < size; ++i) {
            rotationOrder[i] = nullptr; //Set each entry to nullptr
        }
        this->rotationOrderSize = size; //set the rotation order size equal to the parameter 'size'
    }
    else { //A rotationOrder array must already exist
        Quaternion ** tempForNewRotOrderArray = new Quaternion * [size];
        //copy all the existing quaternions into the new array and delete the old array
        for (int i = 0; i < rotationOrderSize; ++i) {
            tempForNewRotOrderArray[i] = rotationOrder[i];
            delete [] rotationOrder;
        }
        //Set any remaining space in the new array to nullptr
        for (int i = rotationOrderSize; i < size; ++i) {
            tempForNewRotOrderArray[i] = nullptr;
        }
        rotationOrder = tempForNewRotOrderArray; //Set rotationOrder to the new array
        rotationOrderSize = size;
        //numberOfRotations should remain unchanged
    }
}

void CollisionBox::addToRotationOrder(const Quaternion & rQuat) { //'rQuat' for rotation quaternion
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    //Check to see if need to create a new rotationOrder array
    if (rotationOrder == nullptr) {
        rotationOrderSize = 1;
        rotationOrder = new Quaternion * [rotationOrderSize];
        rotationOrder[0] = new Quaternion(rQuat); //Use copy constructor
        numberOfRotations = 1;
        return;
    }
    else if (numberOfRotations == rotationOrderSize) { //See if we need to make the rotationOrder array bigger
        Quaternion ** tempForNewRotOrderArray = new Quaternion * [rotationOrderSize + 1];
        //copy all the existing quaternions into the new array and delete them from old array
        for (int i = 0; i < rotationOrderSize; ++i) {
            tempForNewRotOrderArray[i] = rotationOrder[i];
            //Yikes! No! Don't delete them since ownership is just being transfered to the new array
            //if (rotationOrder[i] != nullptr) {
            //delete rotationOrder[i];
            //rotationOrder[i] = nullptr;
            //}
        }
        delete [] rotationOrder;
        tempForNewRotOrderArray[rotationOrderSize] = new Quaternion(rQuat);
        ++rotationOrderSize;
        ++numberOfRotations;
        rotationOrder = tempForNewRotOrderArray; //Set rotationOrder to the new array
        return;
    }
    //There is also the possiblity that the rotationOrder array is full, but with nullspots inbetween it somewhere...
    else if (rotationOrder[rotationOrderSize - 1] != nullptr) { //If there is a valid rotation at the last slot in the array
        //The code to execute for this case is actually exactly the same as the code above it. Might want to combine them?
        Quaternion ** tempForNewRotOrderArray = new Quaternion * [rotationOrderSize + 1];
        //copy all the existing quaternions into the new array and delete them from old array
        for (int i = 0; i < rotationOrderSize; ++i) {
            tempForNewRotOrderArray[i] = rotationOrder[i];
            //Yikes! No! Don't delete them since ownership is just being transfered to the new array
            //if (rotationOrder[i] != nullptr) {
            //delete rotationOrder[i];
            //rotationOrder[i] = nullptr;
            //}
            delete [] rotationOrder;
        }
        tempForNewRotOrderArray[rotationOrderSize] = new Quaternion(rQuat);
        ++rotationOrderSize;
        ++numberOfRotations;
        rotationOrder = tempForNewRotOrderArray; //Set rotationOrder to the new array
        return;
    }
    
    else { //There must be room in the rotation array for more rotations
        int indexOfFurthestRotationInRotOrder = 0;
        //Loop through the entire rotationOrder array and get the largest index that is not null
        for (int i = 0; i < rotationOrderSize; ++i) {
            if (rotationOrder[i] != nullptr) {
                indexOfFurthestRotationInRotOrder = i;
            }
        }
        if (indexOfFurthestRotationInRotOrder == 0 && rotationOrder[0] == nullptr) { //If there were no rotations in the rot order
            rotationOrder[0] = new Quaternion(rQuat); //Then place rQuat as the first rotation
            ++numberOfRotations;
        }
        else if (indexOfFurthestRotationInRotOrder == 0) { //and rotationOrder[0] != nullptr
            rotationOrder[++indexOfFurthestRotationInRotOrder] = new Quaternion(rQuat);
        }
        //If the next available spot is not the final allocated slot in the array
        else if (indexOfFurthestRotationInRotOrder + 1 < rotationOrderSize - 1) {
            rotationOrder[++indexOfFurthestRotationInRotOrder + 1] = new Quaternion(rQuat);
            ++numberOfRotations;
        }
        //We already checked to see if rotationOrder[rotationOrderSize - 1] was not null, so if we are here then we know it is not null
        else {
            rotationOrder[++indexOfFurthestRotationInRotOrder] = new Quaternion(rQuat);
        }
        
        //Print a debug message checking to make sure the right thing happened
        if (printDebugMessages) {
            std::cout << "\nDEBUG::Rotation about axis (" << rQuat.getRotationAxis_x() << ", " << rQuat.getRotationAxis_y() << ", " << rQuat.getRotationAxis_z() << ") added to slot " << indexOfFurthestRotationInRotOrder << " in rotation order." << std::endl;
        }
    }
}
void CollisionBox::changeRotationAt(int index, float theta) {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    if (index >= rotationOrderSize || index < 0) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
            std::cout << "Bad index: " << index << std::endl;
        }
    }
    else if (rotationOrder[index] == nullptr) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! changeRotationAt(Index = " << index;
            std::cout << ") is attempting to modify a null quaternion.\nCheck your Code!\n";
        }
    }
    else {
        rotationOrder[index]->changeTheta(theta);
    }
    doRotationsAndRecalculate();
}
void CollisionBox::changeRotationAxisAt(int index, const Quaternion & rQuat) {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    if (index >= rotationOrderSize || index < 0) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
            std::cout << "Bad index: " << index << std::endl;
        }
        return;
    }
    else { //I could rewrite this whole block in less lines by reversing the check. oh well...
        if (rotationOrder[index] == nullptr) {
            rotationOrder[index] = new Quaternion(rQuat);
        }
        else {
            delete rotationOrder[index];
            rotationOrder[index] = new Quaternion(rQuat);
        }
    }
    doRotationsAndRecalculate();
}
void CollisionBox::changeRotationAxisAt(int index, const aiVector3D & axis, float theta) {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    if (index >= rotationOrderSize || index < 0) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
            std::cout << "Bad index: " << index << std::endl;
        }
        return;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (printDebugMessages) {
                std::cout << "\nDEBUG::OOPS! There is no rotation quat at this index. Your Index: ";
                std::cout << index << std::endl;
            }
        }
        else {
            rotationOrder[index]->changeRotationAxis(axis.x, axis.y, axis.z);
            rotationOrder[index]->changeTheta(theta);
        }
    }
    doRotationsAndRecalculate();
}
void CollisionBox::changeRotationAxisAt(int index, float x, float y, float z, float theta) {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    if (index >= rotationOrderSize || index < 0) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
        }
        return;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (printDebugMessages || printDebugWarnings) {
                std::cout << "\nDEBUG::OOPS! There is no rotation quat at this index. Your Index: ";
                std::cout << index << std::endl;
            }
        }
        else {
            rotationOrder[index]->changeRotationAxis(x, y, z);
            rotationOrder[index]->changeTheta(theta);
        }
    }
    doRotationsAndRecalculate();
}

float CollisionBox::getRotationThetaAt(int index) {
    if (index < 0 || index >= rotationOrderSize) {
        if (printDebugMessages || printDebugWarnings) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO RETRIEVE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
        }
        return 0.0f;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (printDebugMessages || printDebugWarnings) {
                std::cout << "\nDEBUG::OOPS! There is no rotation quat to get a theta value from at\nthis index. Your Index: ";
                std::cout << index << std::endl;
            }
            return 0.0f;
        }
        else {
            return rotationOrder[index]->getTheta();
        }
    }
}

void CollisionBox::clearRotationOrder() {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    for (int i = 0; i < this->rotationOrderSize; ++i) {
        if (rotationOrder[i] != nullptr) {
            delete rotationOrder[i];
            rotationOrder[i] = nullptr;
        }
    }
    numberOfRotations = 0;
    doRotationsAndRecalculate();
}
void CollisionBox::removeRotationAtIndex(int index) {
    useHiddenRotation = false; //Set this to false each time any changes are made to other rotations
    if (index < 0 || index > rotationOrderSize) { //If index given is bogus...
        if (printDebugMessages || printDebugWarnings) { //Print a debug message is debug messages turned on
            std::cout << "\nDEBUG::OOPS! Attempting to remove rotation at index " << index;
            std::cout << " from the\nrotationOrder array in this collisionRectangle!";
            std::cout << "\nThis index is not valid, please enter an index between 0 and ";
            std::cout << rotationOrderSize << std::endl;
        }
        //Throw an exception?
        return;
    }//else
    if (rotationOrder[index] != nullptr) {
        delete rotationOrder[index];
        rotationOrder[index] = nullptr;
        --numberOfRotations;
    }
    else {
        if (printDebugWarnings) {
            std::cout << "\nDEBUG::WARNING! RemoveRotationAtIndex(" << index << ") was called\n";
            std::cout << "on this collisionRectangle, but this collisionRectangle does not have\n";
            std::cout << "a rotation quaternion located at that index. Check to make sure removing\n";
            std::cout <<"from the intended index!" << std::endl;
        }
    }
    doRotationsAndRecalculate();
}

//------------------------------------------------------------------------
//              Collision Detection Functions
//------------------------------------------------------------------------

bool CollisionBox::isWithin(float x, float y) const {
    if (!hasModelData || hasNoArea()) {return false;}
    //I use the test that is outlined in this link: https://stackoverflow.com/questions/5922027/how-to-determine-if-a-point-is-within-a-quadrilateral
    //The idea is that if the point is inside the quadrilateral, then the sum of the areas of the triangles formed from the interior point to the four side points will equal the area of the quadrilateral. If the triangles area sum is greater, then the point is outside the quadrilateral.
    aiVector2D testPoint(x,y);
    float triangleAreaSum = getTriangleArea(corners2D[0], testPoint, corners2D[1]) +
                            getTriangleArea(corners2D[1], testPoint, corners2D[2]) +
                            getTriangleArea(corners2D[2], testPoint, corners2D[3]) +
                            getTriangleArea(corners2D[3], testPoint, corners2D[0]);
    
    return (abs(triangleAreaSum - this->getQuadrilateralArea()) <= 0.00005f); //Just some floating point tolerance in case result is not exactly 0
}

bool CollisionBox::isWithin(const aiVector2D & point) const {
    if (!hasModelData || hasNoArea()) {return false;}
    //See the other version of this function for comments on computation being performed
    float triangleAreaSum = getTriangleArea(corners2D[0], point, corners2D[1]) +
                            getTriangleArea(corners2D[1], point, corners2D[2]) +
                            getTriangleArea(corners2D[2], point, corners2D[3]) +
                            getTriangleArea(corners2D[3], point, corners2D[0]);
    return (abs(triangleAreaSum - this->getQuadrilateralArea()) <= 0.00005f); //Just some floating point tolerance in case result is not exactly 0
}

bool CollisionBox::isOverlapping(const CollisionBox& otherBox) const {
    if (!hasModelData || !(otherBox.hasModelData)) {
        return false;
    }
    //There are several pathological cases I have drawn out on paper (most involving boxes of vastly different size),
    //so I am going to try to get a very general algorithm for checking.
    
    //Idea: It will be easier to do some quick checks to rule out cases where it isn't possible for the two rectangles to be overlapping
    
    //New quick way of checking that takes advantage of the specific rules on how the corners are formed
    
    //Check to see if otherBox's min x coor is greater than this box's max x coor
    if (otherBox.corners2D[2].x > corners2D[0].x) {
        return false;
    }
    //Check to see if the otherBox's min y coor is greater than this box's max y coor
    if (otherBox.corners2D[3].y > corners2D[1].y) {
        return false;
    }
    //Check to see if the otherBox's max x coordinate is less than this box's min x coor
    if (otherBox.corners2D[0].x < corners2D[2].x) {
        return false;
    }
    //Check to see if the otherBox's max y coordinate is less than this box's min y coor
    if (otherBox.corners2D[1].y < corners2D[3].y) {
        return false;
    }
    
    /* //Old inefficient code that tests conditions generally. Since I know there is an ordering to the way
    //   x and y values are assigned to corners, I only need to do one check for each corner instead of 4
     
     //Check to see if all four points of the otherBox rectangle lie beyond the max/min of this rectangles corner coords
     //Start with all points of other having x coords greater than this rectangles x0
     bool allPointsOfOtherBoxBeyondThisBoxExtrema = true;
     
     
    for (int i = 1; i < BOX_CORNERS; ++i) {
        if (otherBox.corners2D[i].x <= corners2D[0].x) { //corners2D[0].x will always be the maximum x of all corners
            allPointsOfOtherBoxBeyondThisBoxExtrema = false;
            break;
        }
    }
    if (allPointsOfOtherBoxBeyondThisBoxExtrema) {return false;} //Then boxes can't be overlapping
    
    //Check to see if all of other's coord are greater than this box's y Max
    allPointsOfOtherBoxBeyondThisBoxExtrema = true;
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.corners2D[i].y <= corners2D[1].y) {
            allPointsOfOtherBoxBeyondThisBoxExtrema = false;
            break;
        }
    }
    if (allPointsOfOtherBoxBeyondThisBoxExtrema) {return false;} //Then boxes can't be overlapping
    
    //Check to see if all of the other box's coord are lesser x values then this box's xMinimun
    allPointsOfOtherBoxBeyondThisBoxExtrema = true;
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.corners2D[i].x >= corners2D[2].x) {
            allPointsOfOtherBoxBeyondThisBoxExtrema = false;
            break;
        }
    }
    if (allPointsOfOtherBoxBeyondThisBoxExtrema) {return false;} //Then boxes can't be overlapping
    
    //Check to see if all of the other box's coord are of lesser y than this boxes yMin coord
    allPointsOfOtherBoxBeyondThisBoxExtrema = true;
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.corners2D[i].y >= corners2D[3].y) {
            allPointsOfOtherBoxBeyondThisBoxExtrema = false;
            break;
        }
    }
    if (allPointsOfOtherBoxBeyondThisBoxExtrema) {return false;} //Then boxes can't be overlapping
    */
    //I haven't definitivly proved it, but I am pretty sure that combining that last check with
    //this next one will always determine if there is any overlap
    
    //Check to see if any of the four corners from one box are inside the other
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.isWithin(this->corners2D[i])) {
            return true;
        }
    }
     
    
    //I can't decide whether to check to see if any of the other box's corners are within this box.
    //Only check to see if this box is overlapping the other, and then have another box check this one if need to check the opposite
//    //Also then do the same check with otherBox's corners inside this one
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (isWithin(otherBox.corners2D[i])) {
            return true;
        }
    }
    return false; //If both tests passed without returning, then boxes are not overlapping
}

aiVector2D CollisionBox::getVectorFromMidpointToPoint(const aiVector2D & point) const {
    if (this->midpoint == point) {
        if (printDebugWarnings) {
            std::cout << "\nDEBUG::Warning! getVectorFromMidpointToPoint called "
                         "with midpoint and point\nequal to same coordinates!\n";
        }
        return aiVector2D(0.0001f, 0.00001f);
    }
    return (point - midpoint);
    
}

void CollisionBox::moveApartAlongAxisBetweenClosestDetectedPoints(CollisionBox & other) {
    int samplePoints;
    if (COLLISION_SAMPLE_POINTS % 2 != 0 || COLLISION_SAMPLE_POINTS < 10) {
        samplePoints = 10;
    }
    else {
        samplePoints = COLLISION_SAMPLE_POINTS;
    }
    
    collisionSamplePointsThis->reserve(samplePoints * 3);
    collisionSamplePointsAll->reserve(samplePoints * samplePoints * 3);
    
}

void CollisionBox::moveApartAlongAxisBetweenMidpoints(CollisionBox & otherBox) {
    if (!(this->hasModelData) || !(otherBox.hasModelData)) {
        if (printDebugWarnings || printDebugMessages) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpointsOtherOnly ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    aiVector2D displacement(this->midpoint.x - otherBox.midpoint.x, this->midpoint.y - otherBox.midpoint.y);
    if (displacement.Length() == 0.0f) {return;} //If there is no displacement, then don't know which direction to move
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        this->midpoint += displacement * STEP_SIZE;
        //Move the other rectangle away also by step amount
        otherBox.midpoint -= displacement * STEP_SIZE;
        this->calculateSelfAfterTranslations();
        otherBox.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherBox) && otherBox.isOverlapping(*this));
}


void CollisionBox::moveApartAlongAxisBetweenMidpointsThisOnly(CollisionBox & otherBox) {
    if (!(this->hasModelData) || !(otherBox.hasModelData)) {
        if (printDebugWarnings || printDebugMessages) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpointsOtherOnly ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    aiVector2D displacement(this->midpoint.x - otherBox.midpoint.x, this->midpoint.y - otherBox.midpoint.y);
    if (displacement.Length() == 0.0f) {return;} //If there is no displacement, then don't know which direction to move
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        this->midpoint += displacement * STEP_SIZE;
        this->calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherBox));
}

void CollisionBox::moveApartAlongAxisBetweenMidpointsOtherOnly(CollisionBox & otherBox) {
    if (!(this->hasModelData) || !(otherBox.hasModelData)) {
        if (printDebugWarnings || printDebugMessages) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpointsOtherOnly ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    aiVector2D displacement(this->midpoint.x - otherBox.midpoint.x, this->midpoint.y - otherBox.midpoint.y);
    if (displacement.Length() == 0.0f) {this->midpoint.x += 0.001f; moveApartAlongAxisBetweenMidpoints(otherBox);} //If there is no displacement, then don't know which direction to move. Fix by forcing a direction
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move the other rectangle away also by step amount
        otherBox.midpoint -= displacement * STEP_SIZE;
        otherBox.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherBox));
}
//------------------------------------------------------------------------
//              Getters for drawing AACollisionBox
//------------------------------------------------------------------------

void CollisionBox::getRotatedMajorMinor3D(float * bufferOfThirtysixFloats) const {
    //Normalize the vectors
    aiVector3D xAxisMajor = this->xAxisMajor;
    //xAxisMajor.NormalizeSafe();
    
    aiVector3D yAxisMajor = this->yAxisMajor;
    //yAxisMajor.NormalizeSafe();
    
    aiVector3D zAxisMajor = this->zAxisMajor;
    //zAxisMajor.NormalizeSafe();
    
    aiVector3D xAxisMinor = this->xAxisMinor;
    //xAxisMinor.NormalizeSafe();
    
    aiVector3D yAxisMinor = this->yAxisMinor;
    //yAxisMinor.NormalizeSafe();
    
    aiVector3D zAxisMinor = this->zAxisMinor;
    //zAxisMinor.NormalizeSafe();
    
    
    bufferOfThirtysixFloats[0] = xAxisMajor.x;
    bufferOfThirtysixFloats[1] = xAxisMajor.y;
    bufferOfThirtysixFloats[2] = xAxisMajor.z;
    bufferOfThirtysixFloats[3] = 0.0f;
    bufferOfThirtysixFloats[4] = 0.0f;
    bufferOfThirtysixFloats[5] = 0.0f;
    
    bufferOfThirtysixFloats[6] = xAxisMinor.x;
    bufferOfThirtysixFloats[7] = xAxisMinor.y;
    bufferOfThirtysixFloats[8] = xAxisMinor.z;
    bufferOfThirtysixFloats[9] = 0.0f;
    bufferOfThirtysixFloats[10] = 0.0f;
    bufferOfThirtysixFloats[11] = 0.0f;
    
    bufferOfThirtysixFloats[12] = yAxisMajor.x;
    bufferOfThirtysixFloats[13] = yAxisMajor.y;
    bufferOfThirtysixFloats[14] = yAxisMajor.z;
    bufferOfThirtysixFloats[15] = 0.0f;
    bufferOfThirtysixFloats[16] = 0.0f;
    bufferOfThirtysixFloats[17] = 0.0f;
    
    bufferOfThirtysixFloats[18] = yAxisMinor.x;
    bufferOfThirtysixFloats[19] = yAxisMinor.y;
    bufferOfThirtysixFloats[20] = yAxisMinor.z;
    bufferOfThirtysixFloats[21] = 0.0f;
    bufferOfThirtysixFloats[22] = 0.0f;
    bufferOfThirtysixFloats[23] = 0.0f;
    
    bufferOfThirtysixFloats[24] = zAxisMajor.x;
    bufferOfThirtysixFloats[25] = zAxisMajor.y;
    bufferOfThirtysixFloats[26] = zAxisMajor.z;
    bufferOfThirtysixFloats[27] = 0.0f;
    bufferOfThirtysixFloats[28] = 0.0f;
    bufferOfThirtysixFloats[29] = 0.0f;
    
    bufferOfThirtysixFloats[30] = zAxisMinor.x;
    bufferOfThirtysixFloats[31] = zAxisMinor.y;
    bufferOfThirtysixFloats[32] = zAxisMinor.z;
    bufferOfThirtysixFloats[33] = 0.0f;
    bufferOfThirtysixFloats[34] = 0.0f;
    bufferOfThirtysixFloats[35] = 0.0f;
}

void CollisionBox::getRect2DCornerPoints2D(float * bufferOfEightFloats) const {
    bufferOfEightFloats[0] = this->corners2D[0].x;
    bufferOfEightFloats[1] = this->corners2D[0].y;
    
    bufferOfEightFloats[2] = this->corners2D[1].x;
    bufferOfEightFloats[3] = this->corners2D[1].y;
    
    bufferOfEightFloats[4] = this->corners2D[2].x;
    bufferOfEightFloats[5] = this->corners2D[2].y;
    
    bufferOfEightFloats[6] = this->corners2D[3].x;
    bufferOfEightFloats[7] = this->corners2D[3].y;
}

void CollisionBox::getRect2DCornerPoints3D(float * bufferOfTwelveFloats) const {
    bufferOfTwelveFloats[0] = this->corners2D[0].x;
    bufferOfTwelveFloats[1] = this->corners2D[0].y;
    bufferOfTwelveFloats[2] = zPlane2D;
    
    bufferOfTwelveFloats[3] = this->corners2D[1].x;
    bufferOfTwelveFloats[4] = this->corners2D[1].y;
    bufferOfTwelveFloats[5] = -zPlane2D;
    
    bufferOfTwelveFloats[6] = this->corners2D[2].x;
    bufferOfTwelveFloats[7] = this->corners2D[2].y;
    bufferOfTwelveFloats[8] = -zPlane2D;
    
    bufferOfTwelveFloats[9] = this->corners2D[3].x;
    bufferOfTwelveFloats[10] = this->corners2D[3].y;
    bufferOfTwelveFloats[11] = -zPlane2D;
}

//DEBUG::This one tested and it works
void CollisionBox::getCubiodTriangles3D(float * bufferOf108Floats) const {
    aiVector3D modelTranslantedCorners3D[8];
    aiVector3D midpoint3D = aiVector3D(midpoint.x, midpoint.y, zMidpointOffset3D);
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        //corners3D[i] = this->corners3D[i];
        modelTranslantedCorners3D[i] = (midpoint3D + this->corners3D[i]) * scale;
    }
    //Triangle 1
    bufferOf108Floats[0] = modelTranslantedCorners3D[0].x;  //yMajor Face
    bufferOf108Floats[1] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[2] = modelTranslantedCorners3D[0].z;
    bufferOf108Floats[3] = modelTranslantedCorners3D[1].x;
    bufferOf108Floats[4] = modelTranslantedCorners3D[1].y;
    bufferOf108Floats[5] = modelTranslantedCorners3D[1].z;
    bufferOf108Floats[6] = modelTranslantedCorners3D[3].x;
    bufferOf108Floats[7] = modelTranslantedCorners3D[3].y;
    bufferOf108Floats[8] = modelTranslantedCorners3D[3].z;
    //Triangle 2
    bufferOf108Floats[9] = modelTranslantedCorners3D[2].x; //yMajor face
    bufferOf108Floats[10] = modelTranslantedCorners3D[2].y;
    bufferOf108Floats[11] = modelTranslantedCorners3D[2].z;
    bufferOf108Floats[12] = modelTranslantedCorners3D[3].x;
    bufferOf108Floats[13] = modelTranslantedCorners3D[3].y;
    bufferOf108Floats[14] = modelTranslantedCorners3D[3].z;
    bufferOf108Floats[15] = modelTranslantedCorners3D[0].x;
    bufferOf108Floats[16] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[17] = modelTranslantedCorners3D[0].z;
    //Triangle 3
    bufferOf108Floats[18] = modelTranslantedCorners3D[2].x; //xMinor face
    bufferOf108Floats[19] = modelTranslantedCorners3D[2].y;
    bufferOf108Floats[20] = modelTranslantedCorners3D[2].z;
    bufferOf108Floats[21] = modelTranslantedCorners3D[3].x;
    bufferOf108Floats[22] = modelTranslantedCorners3D[3].y;
    bufferOf108Floats[23] = modelTranslantedCorners3D[3].z;
    bufferOf108Floats[24] = modelTranslantedCorners3D[7].x;
    bufferOf108Floats[25] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[26] = modelTranslantedCorners3D[7].z;
    //Triangle 4
    bufferOf108Floats[27] = modelTranslantedCorners3D[6].x; //xMinor face
    bufferOf108Floats[28] = modelTranslantedCorners3D[6].y;
    bufferOf108Floats[29] = modelTranslantedCorners3D[6].z;
    bufferOf108Floats[30] = modelTranslantedCorners3D[7].x;
    bufferOf108Floats[31] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[32] = modelTranslantedCorners3D[7].z;
    bufferOf108Floats[33] = modelTranslantedCorners3D[2].x;
    bufferOf108Floats[34] = modelTranslantedCorners3D[2].y;
    bufferOf108Floats[35] = modelTranslantedCorners3D[2].z;
    //Triangle 5
    bufferOf108Floats[36] = modelTranslantedCorners3D[6].x; //yMinor face
    bufferOf108Floats[37] = modelTranslantedCorners3D[6].y;
    bufferOf108Floats[38] = modelTranslantedCorners3D[6].z;
    bufferOf108Floats[39] = modelTranslantedCorners3D[7].x;
    bufferOf108Floats[40] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[41] = modelTranslantedCorners3D[7].z;
    bufferOf108Floats[42] = modelTranslantedCorners3D[4].x;
    bufferOf108Floats[43] = modelTranslantedCorners3D[4].y;
    bufferOf108Floats[44] = modelTranslantedCorners3D[4].z;
    //Triangle 6
    bufferOf108Floats[45] = modelTranslantedCorners3D[5].x; //yMinor face
    bufferOf108Floats[46] = modelTranslantedCorners3D[5].y;
    bufferOf108Floats[47] = modelTranslantedCorners3D[5].z;
    bufferOf108Floats[48] = modelTranslantedCorners3D[4].x;
    bufferOf108Floats[49] = modelTranslantedCorners3D[4].y;
    bufferOf108Floats[50] = modelTranslantedCorners3D[4].z;
    bufferOf108Floats[51] = modelTranslantedCorners3D[7].x;
    bufferOf108Floats[52] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[53] = modelTranslantedCorners3D[7].z;
    //Triangle 7
    bufferOf108Floats[54] = modelTranslantedCorners3D[4].x; //xMajor face
    bufferOf108Floats[55] = modelTranslantedCorners3D[4].y;
    bufferOf108Floats[56] = modelTranslantedCorners3D[4].z;
    bufferOf108Floats[57] = modelTranslantedCorners3D[5].x;
    bufferOf108Floats[58] = modelTranslantedCorners3D[5].y;
    bufferOf108Floats[59] = modelTranslantedCorners3D[5].z;
    bufferOf108Floats[60] = modelTranslantedCorners3D[0].x;
    bufferOf108Floats[61] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[62] = modelTranslantedCorners3D[0].z;
    //Triangle 8
    bufferOf108Floats[63] = modelTranslantedCorners3D[0].x; //xMajor face
    bufferOf108Floats[64] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[65] = modelTranslantedCorners3D[0].z;
    bufferOf108Floats[66] = modelTranslantedCorners3D[1].x;
    bufferOf108Floats[67] = modelTranslantedCorners3D[1].y;
    bufferOf108Floats[68] = modelTranslantedCorners3D[1].z;
    bufferOf108Floats[69] = modelTranslantedCorners3D[5].x;
    bufferOf108Floats[70] = modelTranslantedCorners3D[5].y;
    bufferOf108Floats[71] = modelTranslantedCorners3D[5].z;
    //Triangle 9
    bufferOf108Floats[72] = modelTranslantedCorners3D[0].x; //zMajor face
    bufferOf108Floats[73] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[74] = modelTranslantedCorners3D[0].z;
    bufferOf108Floats[75] = modelTranslantedCorners3D[2].x;
    bufferOf108Floats[76] = modelTranslantedCorners3D[2].y;
    bufferOf108Floats[77] = modelTranslantedCorners3D[2].z;
    bufferOf108Floats[78] = modelTranslantedCorners3D[6].x;
    bufferOf108Floats[79] = modelTranslantedCorners3D[6].y;
    bufferOf108Floats[80] = modelTranslantedCorners3D[6].z;
    //Triangle 10
    bufferOf108Floats[81] = modelTranslantedCorners3D[6].x; //zMajor face
    bufferOf108Floats[82] = modelTranslantedCorners3D[6].y;
    bufferOf108Floats[83] = modelTranslantedCorners3D[6].z;
    bufferOf108Floats[84] = modelTranslantedCorners3D[4].x;
    bufferOf108Floats[85] = modelTranslantedCorners3D[4].y;
    bufferOf108Floats[86] = modelTranslantedCorners3D[4].z;
    bufferOf108Floats[87] = modelTranslantedCorners3D[0].x;
    bufferOf108Floats[88] = modelTranslantedCorners3D[0].y;
    bufferOf108Floats[89] = modelTranslantedCorners3D[0].z;
    //Triangle 11
    bufferOf108Floats[90] = modelTranslantedCorners3D[1].x; //zMinor face
    bufferOf108Floats[91] = modelTranslantedCorners3D[1].y;
    bufferOf108Floats[92] = modelTranslantedCorners3D[1].z;
    bufferOf108Floats[93] = modelTranslantedCorners3D[3].x;
    bufferOf108Floats[94] = modelTranslantedCorners3D[3].y;
    bufferOf108Floats[95] = modelTranslantedCorners3D[3].z;
    bufferOf108Floats[96] = modelTranslantedCorners3D[7].x;
    bufferOf108Floats[97] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[98] = modelTranslantedCorners3D[7].z;
    //Triangle 12
    bufferOf108Floats[99] = modelTranslantedCorners3D[7].x; //zMinor face
    bufferOf108Floats[100] = modelTranslantedCorners3D[7].y;
    bufferOf108Floats[101] = modelTranslantedCorners3D[7].z;
    bufferOf108Floats[102] = modelTranslantedCorners3D[5].x;
    bufferOf108Floats[103] = modelTranslantedCorners3D[5].y;
    bufferOf108Floats[104] = modelTranslantedCorners3D[5].z;
    bufferOf108Floats[105] = modelTranslantedCorners3D[1].x;
    bufferOf108Floats[106] = modelTranslantedCorners3D[1].y;
    bufferOf108Floats[107] = modelTranslantedCorners3D[1].z;
}

//These next two return formated position values for drawing various primatives
void CollisionBox::getRectCornersTriangles3D(float * bufferOfEighteenFloats) const {
    //Triangle 1
    bufferOfEighteenFloats[0] = this->corners2D[0].x;
    bufferOfEighteenFloats[1] = this->corners2D[0].y;
    bufferOfEighteenFloats[2] = -0.5f;
    bufferOfEighteenFloats[3] = this->corners2D[1].x;
    bufferOfEighteenFloats[4] = this->corners2D[1].y;
    bufferOfEighteenFloats[5] = -0.5f;
    bufferOfEighteenFloats[6] = this->corners2D[2].x;
    bufferOfEighteenFloats[7] = this->corners2D[2].y;
    bufferOfEighteenFloats[8] = -0.5f;
    //Triangle 2
    bufferOfEighteenFloats[9] = this->corners2D[2].x;
    bufferOfEighteenFloats[10] = this->corners2D[2].y;
    bufferOfEighteenFloats[11] = -0.5f;
    bufferOfEighteenFloats[12] = this->corners2D[3].x;
    bufferOfEighteenFloats[13] = this->corners2D[3].y;
    bufferOfEighteenFloats[14] = -0.5f;
    bufferOfEighteenFloats[15] = this->corners2D[0].x;
    bufferOfEighteenFloats[16] = this->corners2D[0].y;
    bufferOfEighteenFloats[17] = -0.5f;
    
}

//TESTED THIS ONE AND IT WORKS
void CollisionBox::getRectCornersLines3D(float * bufferOfTwentyfourFloats) const {
    bufferOfTwentyfourFloats[0] = this->corners2D[0].x;
    bufferOfTwentyfourFloats[1] = this->corners2D[0].y;
    bufferOfTwentyfourFloats[2] = -0.5f;
    bufferOfTwentyfourFloats[3] = this->corners2D[1].x;
    bufferOfTwentyfourFloats[4] = this->corners2D[1].y;
    bufferOfTwentyfourFloats[5] = -0.5f;
    
    bufferOfTwentyfourFloats[6] = this->corners2D[1].x;
    bufferOfTwentyfourFloats[7] = this->corners2D[1].y;
    bufferOfTwentyfourFloats[8] = -0.5f;
    bufferOfTwentyfourFloats[9] = this->corners2D[2].x;
    bufferOfTwentyfourFloats[10] = this->corners2D[2].y;
    bufferOfTwentyfourFloats[11] = -0.5f;
    
    bufferOfTwentyfourFloats[12] = this->corners2D[2].x;
    bufferOfTwentyfourFloats[13] = this->corners2D[2].y;
    bufferOfTwentyfourFloats[14] = -0.5f;
    bufferOfTwentyfourFloats[15] = this->corners2D[3].x;
    bufferOfTwentyfourFloats[16] = this->corners2D[3].y;
    bufferOfTwentyfourFloats[17] = -0.5f;
    
    bufferOfTwentyfourFloats[18] = this->corners2D[3].x;
    bufferOfTwentyfourFloats[19] = this->corners2D[3].y;
    bufferOfTwentyfourFloats[20] = -0.5f;
    bufferOfTwentyfourFloats[21] = this->corners2D[0].x;
    bufferOfTwentyfourFloats[22] = this->corners2D[0].y;
    bufferOfTwentyfourFloats[23] = -0.5f;

    //Don't need?
//    bufferOfThirtySixFloats[24] = this->corners2D[].x
//    bufferOfThirtySixFloats[25] = this->corners2D[].y
//    bufferOfThirtySixFloats[26] = 0.5f;
//    bufferOfThirtySixFloats[27] = this->corners2D[].x
//    bufferOfThirtySixFloats[28] = this->corners2D[].y
//    bufferOfThirtySixFloats[29] = 0.5f;
//
//    bufferOfThirtySixFloats[30] = this->corners2D[].x
//    bufferOfThirtySixFloats[31] = this->corners2D[].y
//    bufferOfThirtySixFloats[32] = 0.5f;
//    bufferOfThirtySixFloats[33] = this->corners2D[].x
//    bufferOfThirtySixFloats[34] = this->corners2D[].y
//    bufferOfThirtySixFloats[35] = 0.5f;
}

void CollisionBox::resetCollisionDetectionSamplePoints() {
    if (collisionSamplePointsAll != nullptr) {
        delete collisionSamplePointsAll;
        collisionSamplePointsAll = nullptr; //Don't need this because setting it again on the very next line to a new mem location
    }
    collisionSamplePointsAll = new std::vector<float>;
   
    if (collisionSamplePointsThis != nullptr) {
        delete collisionSamplePointsThis;
        collisionSamplePointsThis = nullptr;
    }
    collisionSamplePointsThis = new std::vector<float>;
}

void CollisionBox::getCollisionDetectionSamplePointsBoxMidpointToBoxMidpoint(float * bufferOfSixFloats, const CollisionBox &otherBox) const {
    bufferOfSixFloats[0] = otherBox.midpoint.x;
    bufferOfSixFloats[1] = otherBox.midpoint.y;
    bufferOfSixFloats[2] = -0.75f;
    bufferOfSixFloats[3] = this->midpoint.x;
    bufferOfSixFloats[4] = this->midpoint.y;
    bufferOfSixFloats[5] = -0.75f;
}

std::vector<float>* CollisionBox::getCollisionDetectionSamplePoints() const {
    if (collisionSamplePointsAll == nullptr) {
        if (printDebugWarnings) {
            std::cout << "\nDEBUG::WARNING collisionSamplePointsAll is null for this collision box!\n";
        }
        return nullptr;
    }
    return collisionSamplePointsAll;
}

std::vector<float>* CollisionBox::getCollisionDetectionSamplePointsThisOnly() const {
    if (collisionSamplePointsThis == nullptr) {
        if (printDebugWarnings) {
            std::cout << "\nDEBUG::WARNING collisionSamplePointsThis is null for this collision box!\n";
        }
        return nullptr;
    }
    return collisionSamplePointsThis;
}

//------------------------------------------------------------------------
//              Private Functions
//------------------------------------------------------------------------
void CollisionBox::initialize() {
    //Set debug variables
    printDebugMessages = false; //Toggle printing out basic information to screen
    printDebugWarnings = true; //Toggle improper use/potentially unintended behavior warnings
    //Initialize object fields
    zPlane2D = -0.5f;
    zMidpointOffset3D = 0.0f;
    hasModelData = false;
    xAxisSymmetry = yAxisSymmetry = zAxisSymmetry = true; //because 0 vector has symmetry
    rotationOrder = nullptr;
    numberOfRotations = 0;
    rotationOrderSize = 0;
    midpoint = aiVector2D(0.0f, 0.0f);
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        corners3D[i] = aiVector3D(0.0f, 0.0f, 0.0f);
    }
    for (int i = 4; i < BOX_CORNERS; ++i) {
        corners2D[i] = aiVector2D(0.0f, 0.0f);
    }
    xAxisMajor = xAxisMinor = yAxisMajor = yAxisMinor = zAxisMajor = zAxisMinor = aiVector3D(0.0f, 0.0f, 0.0f);
    originalMajorsFromModel = originalMinorsFromModel = aiVector3D(0.0f, 0.0f, 0.0f);
    scale = 1.0;
    collisionBoxShrinkageFactor = 1.0f;
    
    //Initialize the adjacencyList indices
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        for (int j = 0; j < 3; ++j) {
            adjacencyList[i][j] = NOT_SET;
        }
    }
    
    //Initialize the vectors for holding the collision sample points
    collisionSamplePointsAll = new std::vector<float>;
    collisionSamplePointsThis = new std::vector<float>;
    
    hiddenRotation = Quaternion(0.0f, 0.0f, 1.0f, FUDGE_FACTOR); //Fudge factor
    useHiddenRotation = false; //Only use the hidden rotation if it is needed
}

void CollisionBox::buildCornerAdjacencyList() { //Note that this function must be called before any rotations occur
    if (!hasModelData) {return;}
    //It turns out that no matter which way the axes are set, the adjacency list remains the same
    adjacencyList[0][0] = 1;  //The list is derived by how the corners3D array will be set in setCorners3D()
    adjacencyList[0][1] = 2;  //
    adjacencyList[0][2] = 4;  //
    //
    adjacencyList[1][0] = 0;
    adjacencyList[1][1] = 3;
    adjacencyList[1][2] = 5;
    //
    adjacencyList[2][0] = 0;
    adjacencyList[2][1] = 3;
    adjacencyList[2][2] = 6;
    //
    adjacencyList[3][0] = 1;
    adjacencyList[3][1] = 2;
    adjacencyList[3][2] = 7;
    //
    adjacencyList[4][0] = 0;
    adjacencyList[4][1] = 5;
    adjacencyList[4][2] = 6;
    //
    adjacencyList[5][0] = 1;
    adjacencyList[5][1] = 4;
    adjacencyList[5][2] = 7;
    //
    adjacencyList[6][0] = 2;
    adjacencyList[6][1] = 4;
    adjacencyList[6][2] = 7;
    //
    adjacencyList[7][0] = 3;
    adjacencyList[7][1] = 5;
    adjacencyList[7][2] = 6;
}

void CollisionBox::setCorners3D() {
    corners3D[0] = xAxisMajor + yAxisMajor + zAxisMajor;
    corners3D[1] = xAxisMajor + yAxisMajor + zAxisMinor;
    corners3D[2] = xAxisMajor + yAxisMinor + zAxisMajor;
    corners3D[3] = xAxisMajor + yAxisMinor + zAxisMinor;
    corners3D[4] = xAxisMinor + yAxisMajor + zAxisMajor;
    corners3D[5] = xAxisMinor + yAxisMajor + zAxisMinor;
    corners3D[6] = xAxisMinor + yAxisMinor + zAxisMajor;
    corners3D[7] = xAxisMinor + yAxisMinor + zAxisMinor;
}

void CollisionBox::doRotationsAndRecalculate() {
    if (!hasModelData) {return;}
    //Set the major axes (these are the largest values along each axis for the model
    xAxisMajor = aiVector3D(originalMajorsFromModel.x, 0.0f, 0.0f);
    yAxisMajor = aiVector3D(0.0f, originalMajorsFromModel.y, 0.0f);
    zAxisMajor = aiVector3D(0.0f, 0.0f, originalMajorsFromModel.z);
    //Set the minor axes (these are the largest values of opposite sign from the majors of the model)
    xAxisMinor = aiVector3D(originalMinorsFromModel.x, 0.0f, 0.0f);
    yAxisMinor = aiVector3D(0.0f, originalMinorsFromModel.y, 0.0f);
    zAxisMinor = aiVector3D(0.0f, 0.0f, originalMinorsFromModel.z);
    
    if (printDebugMessages) {
        std::cout << "\nDEBUG::MAJOR AXIS VALUES OF MODEL: ";
        std::cout << originalMajorsFromModel.x << ", ";
        std::cout << originalMajorsFromModel.y << ", ";
        std::cout << originalMajorsFromModel.z << std::endl;
        std::cout << "       MINOR AXIS VALUES OF MODEL: ";
        std::cout << originalMinorsFromModel.x << ", ";
        std::cout << originalMinorsFromModel.y << ", ";
        std::cout << originalMinorsFromModel.z << std::endl;
    }
    //Now to rotate all of these vectors
    for (int i = 0; i < rotationOrderSize; ++i) {
        if (rotationOrder[i] != nullptr) { //If a valid rotation exists in the rotationOrder at this index
            //do this rotation on all 6 vectors
            xAxisMajor = rotationOrder[i]->computeRotation(xAxisMajor);
            xAxisMinor = rotationOrder[i]->computeRotation(xAxisMinor);
            yAxisMajor = rotationOrder[i]->computeRotation(yAxisMajor);
            yAxisMinor = rotationOrder[i]->computeRotation(yAxisMinor);
            zAxisMajor = rotationOrder[i]->computeRotation(zAxisMajor);
            zAxisMinor = rotationOrder[i]->computeRotation(zAxisMinor);
            
            if (printDebugMessages) {
                std::cout << "Performed rotation #" << i << " on collision box. ";
                std::cout << "Rotated by: " << rotationOrder[i]->getTheta() << "\n";
                aiVector3D rotAxis = rotationOrder[i]->getRotationAxis();
                std::cout << "Rotation axis of rotation " << i << " is: ";
                std::cout << rotAxis.x << ", " << rotAxis.y << ", " << rotAxis.z;
                std::cout << std::endl;
            }
        }
    }
    if (useHiddenRotation) { //Then do the hidden rotation at the end
        xAxisMajor = hiddenRotation.computeRotation(xAxisMajor);
        xAxisMinor = hiddenRotation.computeRotation(xAxisMinor);
        yAxisMajor = hiddenRotation.computeRotation(yAxisMajor);
        yAxisMinor = hiddenRotation.computeRotation(yAxisMinor);
        zAxisMajor = hiddenRotation.computeRotation(zAxisMajor);
        zAxisMinor = hiddenRotation.computeRotation(zAxisMinor);
    }
    setCorners3D(); //Set the 3D corners array to the rotated values
    calculateSelfAfterTranslations();
}

void CollisionBox::calculateSelfAfterTranslations() {
    if (!hasModelData) {return;}
    //We know that corners3D will have already been set, so construct 2D box directly from corners3D
    aiVector3D modelTranslatedCorners3D[8];
    aiVector3D midpoint3D = aiVector3D(midpoint.x, midpoint.y, 0.0f);
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        //corners3D[i] = this->corners3D[i];
        modelTranslatedCorners3D[i] = (midpoint3D + (this->corners3D[i] * collisionBoxShrinkageFactor))  * scale;
    }
    
    float maxX, minX, maxY, minY;
    maxX = minX = scale * midpoint.x;
    maxY = minY = scale * midpoint.y;
    int maxXIndex, minXIndex, maxYIndex, minYIndex;
    maxXIndex = minXIndex = maxYIndex = minYIndex = NOT_SET;
    
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        if (modelTranslatedCorners3D[i].x >= maxX) {
            maxX = modelTranslatedCorners3D[i].x;
            maxXIndex = i;
        }
        else if (modelTranslatedCorners3D[i].x <= minX) {
            minX = modelTranslatedCorners3D[i].x;
            minXIndex = i;
        }
        
        if (modelTranslatedCorners3D[i].y >= maxY) {
            maxY = modelTranslatedCorners3D[i].y;
            maxYIndex = i;
        }
        else if (modelTranslatedCorners3D[i].y <= minY) {
            minY = modelTranslatedCorners3D[i].y;
            minYIndex = i;
        }
    }
    

    corners2D[0] = aiVector2D(modelTranslatedCorners3D[maxXIndex].x, modelTranslatedCorners3D[maxXIndex].y); //Max x
    corners2D[1] = aiVector2D(modelTranslatedCorners3D[maxYIndex].x, modelTranslatedCorners3D[maxYIndex].y); //Max y
    corners2D[2] = aiVector2D(modelTranslatedCorners3D[minXIndex].x, modelTranslatedCorners3D[minXIndex].y); //Min x
    corners2D[3] = aiVector2D(modelTranslatedCorners3D[minYIndex].x, modelTranslatedCorners3D[minYIndex].y); //Min y

    //Note that constructing this way causes issues if two points tied for same max/min x or y value
    //So to fix this boundary condition, check to see if any two corners were set to the same coordinates
    if ((corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y) || //Don't need to check corners on opposite sides
        (corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y) ||
        (corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) ||
        (corners2D[3].x == corners2D[0].x && corners2D[3].y == corners2D[0].y) ) {
        
        if (!useHiddenRotation) { //If it failed and we haven't tried the hidden rotation yet, try the hidden rotation
            useHiddenRotation = true;
            doRotationsAndRecalculate(); //Redo the 2D box formation process with the hidden rotation included
        }
        
        //Else if we already used the hidden-rotation fix and the box that still wasn't formed correctly....
        else { //The hidden rotation must have failed, so need to try to fix it another way
            //Check all possible cases of how the box is degenerate and try to fix each one
            //NOTE: Since there are 4 corners, there are (11.5) different cases of degeneracy we need to check for
            
            //Debug for testing cases:
//            corners2D[0].x = -0.15f;     corners2D[0].y = 0.0f;
//            corners2D[1].x = -0.15f;     corners2D[1].y = 0.15f;
//            corners2D[2].x = -0.15f;     corners2D[2].y = 0.0f;
//            corners2D[3].x = -0.15f;     corners2D[3].y = -0.15f;
            
            //Case 1: (NOTE THAT CASE 1 WILL BE LEAST LIKELY TO OCCUR, BUT NEED TO CHECK IT FIRST TO RULE IT OUT)
            //See if all 4 points are the same point (WORST CASE - FIX WILL NOT BE IDEAL)
            if (corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y &&
                corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y &&
                corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) {
                //Then we are screwed because all four corners were set to the same value...
                //See if the corners are equal to the midpoint or orgin
                if (corners2D[0].x == midpoint3D.x && corners2D[0].y == midpoint3D.y) {
                    //then just make a really really tiny box to represent just the midpoint in space
                    corners2D[0].x += 0.001f;  // Then form a box around the midpoint that looks like:
                    corners2D[0].y += 0.001f;  //   corner1 +------------+  corner0
                    corners2D[1].x -= 0.001f;  //           |            |
                    corners2D[1].y += 0.001f;  //           |            |
                    corners2D[2].x -= 0.001f;  //           |     mid    |       This box follows the [0].x >= [2].x &&
                    corners2D[2].y -= 0.001f;  //           |            |                            [1].y >= [3].y  requirement
                    corners2D[3].x += 0.001f;  //           |            |
                    corners2D[3].y -= 0.001f;  //   corner2 +------------+  corner3
                }
                //Case 1.5:
                else { //Else they all got set to a corner that is not equal to the midpoint
                    //Rotate this corner around the midpoint to create a square (which won't be perfect, but better than nothing)
                    aiVector2D translation = aiVector2D(midpoint3D.x, midpoint3D.y);
                    
                    //Need to create a 3D vector for doing rotation
                    aiVector3D tempForRotating(corners2D[0].x - translation.x, corners2D[0].y - translation.y, 0.0f);
                    Quaternion rotation(0.0f, 0.0f, 1.0f, PI/2.0f);
                    
                    //Rotate 90 degrees (pi/2 radians)
                    tempForRotating = rotation.computeRotation(tempForRotating);
                    //Set corner 1
                    corners2D[1] = aiVector2D(tempForRotating.x + translation.x, tempForRotating.y + translation.y);
                    
                    //Rotate 180 degrees (pi radians)
                    tempForRotating = rotation.computeRotation(tempForRotating);
                    //set corner 2
                    corners2D[2] = aiVector2D(tempForRotating.x + translation.x, tempForRotating.y + translation.y);
                    
                    //Rotate 270 degrees (3 pi / 2 radians)
                    tempForRotating = rotation.computeRotation(tempForRotating);
                    //set corner 3
                    corners2D[3] = aiVector2D(tempForRotating.x + translation.x, tempForRotating.y + translation.y);
                    
                    //Print a message announcing that degenerate case occured:
                    //std::cout << "\nAll corners set to point that was not midpoint!" << std::endl;
                }
            }
            //Else see if 3 corners got set to the same point
            //Case 2:
            //If 0 1 2 are the same
            else if (corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y &&
                corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y) {
                std::cout << "\nCorners 0 1 2 set to same point!\n";
            }
            //Case 3:
            //If 0 1 3 are the same
            else if (corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y &&
                     corners2D[1].x == corners2D[3].x && corners2D[1].y == corners2D[3].y) {
                std::cout << "\nCorners 0 1 3 set to same point!\n";
            }
            //Case 4:
            //If 0 2 3 are the same
            //
            //   What this would look like:                To Fix, Push corners 0 and 2 out by distance between 1 and 3
            //
            //                  •  Corner 1                             _•_ corner 1
            //                  |                                     _/   \_
            //                  |                                  _/         \_
            //                  |                                _/             \_
            //                  |                     corner 2 •---------•---------• corner 0
            //                  |                                     corner 3
            //                  |
            //                  |
            //         corner 2 • corner 0
            //               corner 3
            //
            //
            //
            //
            
            else if (corners2D[0].x == corners2D[2].x && corners2D[0].y == corners2D[2].y &&
                     corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) {
                std::cout << "\nCorners 0 2 3 set to same point!\n";
            }
            //Case 5:
            //If 1 2 3 are the same
            else if (corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y &&
                     corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) {
                std::cout << "\nCorners 1 2 3 set to same point!\n";
            }
            
            //Else check to see if just 2 corners were set to the same point
            //Case 6:
            //If just 0 and 1 are the same
            else if (corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y) {
                //std::cout << "\nCorners 0 1 set to same point!\n";
                //This case is now handled (for the most part)
                
                //Do a quick direct fix to fix the specific problem I am experiencing. Can do more detailed fix later...
                //Here is what is going wrong:
                //
                //        Corner 2  +-------+  Corners 0 and 1
                //                  |       /
                //                  |      /
                //                  |     /
                //                  |    /
                //                  |   /
                //                  |  /
                //        Corner 3  + /
                //
                
                //So just do this:
                corners2D[0].y = corners2D[3].y;
                
                
                //fixTwoCornersSetSame(0, 1, 2, 3); //First 2 parameters are the corners that are equal
            }
            
            //Case 7:
            // What I wrote below is wrong. Consider the case:
            //                  • corner 1                    --+--
            //                  |                               |
            //                  |                               |  distance == abs (corner1.y - corner3.y)
            //                  |                               |
            //         corner 2 • corner 0                      |
            //                  |                               |
            //                  |                               |
            //                  |                               |
            //         corner 3 •                             --+--
            //
            // Solution:
            //                 _•_ corner 1
            //               _/   \_
            //            _/         \_
            //          _/             \_                       (push the middle corners out)
            //corner 2 •                 • corner 0
            //          \_             _/
            //            \_         _/
            //              \_     _/
            //        corner 3 -•-
            //
            //
            //
            //
            else if (corners2D[0].x == corners2D[2].x && corners2D[0].y == corners2D[2].y) {
                std::cout << "\nA collision box was ill-formed, even after correction!\n";
                //Find the distance between corners 1 and 3
                //Note that we know the x coord of 1 and 3 has to be the same
                ////Reason: cor2D[2].x <= cor2D[1].x <= cor2D[0].x  and   cor2D[2].x <= cor2D[3].x <= cor2D[0].x
                //and we know that cor2D[2].x == cor2D[0].x
                
                //Thus distance will just be the absolute value of their y coordinates
                float distance_c1c3 = abs(corners2D[1].y - corners2D[3].y);
                //Multiply corners 0 and 2 by hald this distance to form a quadrilateral (more specifically, a square)
                
                //First, move corners2D[0] and [2] to be at the orgin
//                float xDistanceToOrgin = corners2D[0].x; //just take any corners x value because all x values will be equal
//                corners2D[0].x -= xDistanceToOrgin;
//                corners2D[2].x -= xDistanceToOrgin;
                //Then scale them outwards (don't multiply because their x component is 0
                corners2D[0].x += 0.5f * distance_c1c3;
                corners2D[2].x -= 0.5f * distance_c1c3;
                //Then translate them back
//                corners2D[0].x += xDistanceToOrgin;
//                corners2D[2].x += xDistanceToOrgin;
                
                //Idea: Add a part to this class that tests to see if the box is a square, rectangle, parallolgram or quadrilateral (test in that order). If class is a square, then do this method I wrote above. If class is a rectangle, then the first time it is created, compute it's area. Have this area always be a few frames behind the actual moodel position/orientation, and then if a degenerate case arises, try to recreate the rectangle off the stored area. Do the same thing for the parallelogram case. If no area has been calculated yet, do the square case but don't save it's area. If shape is a quadrilateral, then store like the ratio of its sides, or something (think more about the quadrilateral case. see https://math.stackexchange.com/questions/1033834/finding-the-fourth-vertex-of-a-quadrilateral-of-given-area For quadrilateral area matching...
                
                //  see also: https://stackoverflow.com/questions/3306838/algorithm-for-reflecting-a-point-across-a-line for reflecting points across lines
                
                
            }
            
            
            //Old comment from before I had considered this case thouroughly:
            // //If just 0 and 2 are the same  (I don't think this case should ever happen...)
            // //Reason: cor2D[2].x must be <= cor2D[1].x <= cor2D[0].x  and   cor2D[2].x must be <= cor2D[3].x <= cor2D[0].x
            // So it is impossible for 1 and 3 to have x coordinates
//             else if (corners2D[0].x == corners2D[2].x && corners2D[0].y == corners2D[2].y) {
//                 std::cout << "\nCorners 0 2 set to same point!\n";
//                 fixTwoCornersSetSame(0, 2, 1, 3);
//             }
            
            //Case 8:
            //If just 0 and 3 are the same
            else if (corners2D[0].x == corners2D[3].x && corners2D[0].y == corners2D[3].y) {
                //std::cout << "\nCorners 0 3 set to same point!\n";
                //fixTwoCornersSetSame(0, 3, 1, 2);
                
                corners2D[3].x = corners2D[2].x;
            }
            //Case 9:
            //if just 1 and 2 are the same
            else if (corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y) {
                
                if (corners2D[0].y == corners2D[1].y) {
                    corners2D[2].y = corners2D[3].y;
                }
                else {
                    corners2D[1].x = corners2D[0].x;
                }
                //std::cout << "\nCorners 1 2 set to same point!\n";
                //std::cout << "Fixed coordinates: [0] = <" << corners2D[0].x << "," << corners2D[0].y << ">" << std::endl;
                //std::cout << "                   [1] = <" << corners2D[1].x << "," << corners2D[1].y << ">"<< std::endl;
                //std::cout << "                   [2] = <" << corners2D[2].x << "," << corners2D[2].y << ">" << std::endl;
                //std::cout << "                   [3] = <" << corners2D[3].x << "," <<corners2D[3].y << ">" << std::endl;
                //fixTwoCornersSetSame(1, 2, 0, 3);
                
                
            }
            //case 10:
            //I have a proof of why this case will never happen, but it's too large for the comment margin here...
            //if just 1 and 3 are the same (I don't think this case should ever happen...)
            //else if (corners2D[1].x == corners2D[3].x && corners2D[1].y == corners2D[3].y) {
            //    std::cout << "\nCorners 1 3 set to same point!\n";
            //    fixTwoCornersSetSame(1, 3, 0, 2);
            //}
            //case 11:
            //if just 2 and 3 are the same
            else if (corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) {
                //std::cout << "\nCorners 2 3 set to same point!\n";
                //fixTwoCornersSetSame(2, 3, 0, 1);
            }
            
            else {
                std::cout << "\nThe box did not form correctly and it did so in a way that the code did not handle!\n";
            }
        }
        
        //This was my old way that didn't quite work, but it made sure code always ran...
//        else { //I HAD a bug in my code and this here was to try to fix it...
//            //This may look like repeated code with the above, but here I am checking each case seperatly
//            //The hidden rotation didn't work, so need to change the shape even more (note this should rarely need to happen)
//            if ( corners2D[0].x == corners2D[1].x && corners2D[0].y == corners2D[1].y) {
//                corners2D[0].x += 0.0001f;
//                corners2D[0].y += 0.0001f;
//            }
//            if (corners2D[1].x == corners2D[2].x && corners2D[1].y == corners2D[2].y) {
//                corners2D[1].x -= 0.0001f;
//                corners2D[1].y -= 0.0001f;
//            }
//            if ( corners2D[2].x == corners2D[3].x && corners2D[2].y == corners2D[3].y) {
//                corners2D[2].x += 0.0001f;
//                corners2D[2].y += 0.0001f;
//            }
//            else if (corners2D[3].x == corners2D[0].x && corners2D[3].y == corners2D[0].y) {
//                corners2D[3].x -= 0.0001f;
//                corners2D[3].y -= 0.0001f;
//            }
//            return; //Don't redo rotations and recalculate...
//        }
        
    }
}

void CollisionBox::fixTwoCornersSetSame(int same1, int same2, int diff1, int diff2) {
    //Quick fix algorithm that could be better:
    aiVector2D vecFromDoubleCornerToMidpoint( -corners2D[same1].x,  -corners2D[same1].y);
    //Just translate one of the double corners by 2 times this vector
    corners2D[same1] += (.002f * vecFromDoubleCornerToMidpoint);
    return;
    
    //Idea for better algorithm:
    //Get lines/vectors between the three points
    //Figure out which of these lines is closest to the midpoint
    //Reflect the degenerate corner over this closest line to get the fourth point
    
    //I (could) used this as a reference:
    // https://stackoverflow.com/questions/3306838/algorithm-for-reflecting-a-point-across-a-line
    
    //ANother idea: Could see if the quadrilateral had an area the previous frame, and then from that
    //use it to calculate the new box.
    //See also: https://math.stackexchange.com/questions/1033834/finding-the-fourth-vertex-of-a-quadrilateral-of-given-area
    //Actually heck if I am going with the area-storing route, why not just save whatever the quadrilateral's points were on
    //the previous frame and then if the box formation fails, just use the coordinates from the last frame (but like translate them
    //and all that...)
}

bool CollisionBox::hasNoArea() const {
    if (!hasModelData) {return true;}
    //Math note: This can be used to determine if three points are colinear (might be overkill?):
    //                http://mathworld.wolfram.com/Cayley-MengerDeterminant.html
    return (getQuadrilateralArea() <= FUDGE_FACTOR); //See if the quad area is (nearly) 0.0f
}

float CollisionBox::getQuadrilateralArea() const {
    if (!hasModelData) {
        if (printDebugWarnings || printDebugMessages) {
            std::string message = " \nDEBUG::WARNING!:"
            "getQuadrilateralArea called on this collision box\n"
            "yet this collision box never had model data set to it!" ;
            std::cout << message << std::endl;
            return FUDGE_FACTOR; //Return a small number to prevent potential Divide By Zero errors
        }
    }
    //(x0,y0) will be point with maximum x value, (x1,y1) will be point with max y value, (x2,y2) min x value, (x3,y3) min y val
    float x0 = corners2D[0].x; //So 0 and 2 will be on opposite sides
    float y0 = corners2D[0].y;
    float x1 = corners2D[1].x; //and 1 and 3 will be on opposite sides
    float y1 = corners2D[1].y;
    float x2 = corners2D[2].x;
    float y2 = corners2D[2].y;
    float x3 = corners2D[3].x;
    float y3 = corners2D[3].y;
    
    //Could use the Cayley-Menger determinant, or could just be simple and calculate the area of two triangles and add
    //(technically these next 2 computations are themselves determinants) see: http://mathworld.wolfram.com/TriangleArea.html
    float triangle1Area = abs((-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2)); //Note this is double the triangle area
    float triangle2Area = abs((-x3*y0 + x2*y0 + x0*y3 - x2*y3 - x0*y2 + x3*y2)); //I will just wait till end to divide by 2
    return ((abs(triangle1Area) + abs(triangle2Area)) / 2.0f);
}


/*!
This Code is for finding the largest absolute value in a collection of floats.
@remark This just returns the maximum value in an array of floats using absolue value
*/
float CollisionBox::getMaxFromArray(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (abs(data[i]) > max) {
            max = abs(data[i]);
        }
    }
    return max;
}

float CollisionBox::getMaxFromArrayPositiveOnly(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        //if (data[i] < 0.0f) {continue;} //
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}
//Finds the most negative value in an array of floats
float CollisionBox::getMaxFromArrayNegativeOnly(float * data, int dataSize) {
    float negMax = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (data[i] < negMax) {
            negMax = data[i];
        }
    }
    return negMax;
}
