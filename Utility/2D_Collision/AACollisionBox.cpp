//
//  AACollisionBox.cpp
//
//  Created by Forrest Miller on 3/5/18.
//

#include "AACollisionBox.h"

static const float STEP_SIZE = 0.01f; //Used for seperating two overlapping CollisionBoxes

static const int CUBOID_CORNERS = 8;
static const int BOX_CORNERS = 4;
static const int NOT_SET = -1;

//------------------------------------------------------------------------
//              CONSTRUCTORS
//------------------------------------------------------------------------
AACollisionBox::AACollisionBox() {
    initialize();
}

/*!
 Constructor that initializes the AACollisionBox straight from the model data.
 If model has no z component, just create an array with x and y coordinates as
 desired and set every third value to 0.0f.
 @remark  The expected format of the array Data is model data in the form:
 x0,y0,z0,x1,y1,z1,x2,y2,z2,...
 */
AACollisionBox::AACollisionBox(float * data, int dataPoints) {
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
    
    
    //Figure out which direction along each basis axis will be major and which will be minor
    //Along the x axis:
    if (maxPosX == abs(maxNegX)) {
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
    }
    else if (maxPosX > abs(maxNegX)) { //Else if positive x values have larger max then neg x values
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
    }
    else { //else negative x must have contained overall larger values
        //use negative as major
        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxNegX;
        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxPosX;
    }
    
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

AACollisionBox::AACollisionBox(aiVector3D * data, int numberOfVectors) {
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
    
    doRotationsAndRecalculate(); //This will set up the box based off the axis data
}

//------------------------------------------------------------------------
//              DESTRUCTOR
//------------------------------------------------------------------------
AACollisionBox::~AACollisionBox() {
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
}

//------------------------------------------------------------------------
//              Set Model Data Function
//------------------------------------------------------------------------
void AACollisionBox::setFromModelData(float * data, int dataPoints) {
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
    
    //Initialize the large array of corner combinations
    for (int i = 0; i < 70; ++i) {
        for (int j = 0; j < 4; ++j){
            corners2DPossibleCombinations[i][j] = aiVector2D(0.0f, 0.0f);
        }
    }
    buildCornerAdjacencyList(); //Build this list after model data is set
    doRotationsAndRecalculate(); //This will set up the box based off the axis data
}

//------------------------------------------------------------------------
//              Rotation Functions
//------------------------------------------------------------------------
void AACollisionBox::presetRotationOrderSize(int size) {
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

void AACollisionBox::addToRotationOrder(const Quaternion & rQuat) { //'rQuat' for rotation quaternion
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
void AACollisionBox::changeRotationAt(int index, float theta) {
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
void AACollisionBox::changeRotitationAxisAt(int index, const Quaternion & rQuat) {
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
void AACollisionBox::changeRotitationAxisAt(int index, const aiVector3D & axis, float theta) {
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
void AACollisionBox::changeRotitationAxisAt(int index, float x, float y, float z, float theta) {
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

float AACollisionBox::getRotationThetaAt(int index) {
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

void AACollisionBox::clearRotationOrder() {
    for (int i = 0; i < this->rotationOrderSize; ++i) {
        if (rotationOrder[i] != nullptr) {
            delete rotationOrder[i];
            rotationOrder[i] = nullptr;
        }
    }
    numberOfRotations = 0;
    doRotationsAndRecalculate();
}
void AACollisionBox::removeRotationAtIndex(int index) {
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

bool AACollisionBox::isWithin(float x, float y) const {
    if (!hasModelData || hasNoArea()) {return false;}
    return false;
}

bool AACollisionBox::isWithin(const aiVector2D & point) const {
    if (!hasModelData || hasNoArea()) {return false;}
    return false;
}

bool AACollisionBox::isOverlapping(const AACollisionBox& otherBox) const {
    if (!hasModelData || !(otherBox.hasModelData)) {
        return false;
    }
    //Actually neither of these next 2 are special cases... four points could be (nearly) colinear (maybe, probably not?)
    //Consider the case of very small values along one axis relative to another...
//    else if (this->getQuadrilateralArea() == 0) {
    //        //check to see if any three points of this quadrilateral are inside the other (problem: which 3?)
//    }
//    else if (otherBox.getQuadrilateralArea() == 0) {
//
//    }
    else { //Check to see if any of the four corners from one box are inside the other
        if (otherBox.isWithin(this->corners2D[0])) {
            return true;
        }
        if (otherBox.isWithin(this->corners2D[1])) {
            return true;
        }
        if (otherBox.isWithin(this->corners2D[2])) {
            return true;
        }
        if (otherBox.isWithin(this->corners2D[3])) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------
//              Getters for drawing AACollisionBox
//------------------------------------------------------------------------

void AACollisionBox::getRotatedMajorMinor3D(float * bufferOfThirtysixFloats) const {
    //Normalize the vectors
    aiVector3D xAxisMajor = this->xAxisMajor;
    xAxisMajor.NormalizeSafe();
    
    aiVector3D yAxisMajor = this->yAxisMajor;
    yAxisMajor.NormalizeSafe();
    
    aiVector3D zAxisMajor = this->zAxisMajor;
    zAxisMajor.NormalizeSafe();
    
    aiVector3D xAxisMinor = this->xAxisMinor;
    xAxisMinor.NormalizeSafe();
    
    aiVector3D yAxisMinor = this->yAxisMinor;
    yAxisMinor.NormalizeSafe();
    
    aiVector3D zAxisMinor = this->zAxisMinor;
    zAxisMinor.NormalizeSafe();
    
    
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

void AACollisionBox::getRect2DCornerPoints2D(float * bufferOfEightFloats) const {
    bufferOfEightFloats[0] = this->corners2D[0].x;
    bufferOfEightFloats[1] = this->corners2D[0].y;
    
    bufferOfEightFloats[2] = this->corners2D[1].x;
    bufferOfEightFloats[3] = this->corners2D[1].y;
    
    bufferOfEightFloats[4] = this->corners2D[2].x;
    bufferOfEightFloats[5] = this->corners2D[2].y;
    
    bufferOfEightFloats[6] = this->corners2D[3].x;
    bufferOfEightFloats[7] = this->corners2D[3].y;
}

void AACollisionBox::getRect2DCornerPoints3D(float * bufferOfTwelveFloats) const {
    bufferOfTwelveFloats[0] = this->corners2D[0].x;
    bufferOfTwelveFloats[1] = this->corners2D[0].y;
    bufferOfTwelveFloats[2] = -0.5f;
    
    bufferOfTwelveFloats[3] = this->corners2D[1].x;
    bufferOfTwelveFloats[4] = this->corners2D[1].y;
    bufferOfTwelveFloats[5] = -0.5f;
    
    bufferOfTwelveFloats[6] = this->corners2D[2].x;
    bufferOfTwelveFloats[7] = this->corners2D[2].y;
    bufferOfTwelveFloats[8] = -0.5f;
    
    bufferOfTwelveFloats[9] = this->corners2D[3].x;
    bufferOfTwelveFloats[10] = this->corners2D[3].y;
    bufferOfTwelveFloats[11] = -0.5f;
}
void AACollisionBox::getRect3DCornerPoints3D(float * bufferOfTwentyfourFloats) const {
    bufferOfTwentyfourFloats[0] = corners3D[1].x;
    bufferOfTwentyfourFloats[1] = corners3D[1].y;
    bufferOfTwentyfourFloats[2] = corners3D[1].z;
    
    bufferOfTwentyfourFloats[3] = corners3D[0].x;
    bufferOfTwentyfourFloats[4] = corners3D[0].y;
    bufferOfTwentyfourFloats[5] = corners3D[0].z;
    
    bufferOfTwentyfourFloats[6] = corners3D[3].x;
    bufferOfTwentyfourFloats[7] = corners3D[3].y;
    bufferOfTwentyfourFloats[8] = corners3D[3].z;
    
    bufferOfTwentyfourFloats[9] = corners3D[2].x;
    bufferOfTwentyfourFloats[10] = corners3D[2].y;
    bufferOfTwentyfourFloats[11] = corners3D[2].z;
    
    bufferOfTwentyfourFloats[12] = corners3D[6].x;
    bufferOfTwentyfourFloats[13] = corners3D[6].y;
    bufferOfTwentyfourFloats[14] = corners3D[6].z;
    
    bufferOfTwentyfourFloats[15] = corners3D[4].x;
    bufferOfTwentyfourFloats[16] = corners3D[4].y;
    bufferOfTwentyfourFloats[17] = corners3D[4].z;
    
    bufferOfTwentyfourFloats[18] = corners3D[5].x;
    bufferOfTwentyfourFloats[19] = corners3D[5].y;
    bufferOfTwentyfourFloats[20] = corners3D[5].z;
    
    bufferOfTwentyfourFloats[21] = corners3D[7].x;
    bufferOfTwentyfourFloats[22] = corners3D[7].y;
    bufferOfTwentyfourFloats[23] = corners3D[7].z;
}

//These next two return formated position values for drawing various primatives
void AACollisionBox::getRectCornersTriangles3D(float * bufferOfEighteenFloats) const {
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
void AACollisionBox::getRectCornersLines3D(float * bufferOfTwentyfourFloats) const {
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


void getCollisionDetectionSamplePointsBoxToBox(float * bufferOf200Floats);
void getCollisionDetectionSamplePointsBoxToBoxMidpoint(float * bufferOf200Floats);
void getCollisionDetectionSamplePointsBoxMidpointToBoxMidpoint(float * bufferOfFourFloats);


//------------------------------------------------------------------------
//              Private Functions
//------------------------------------------------------------------------
void AACollisionBox::initialize() {
    //Set debug variables
    printDebugMessages = false; //Toggle printing out basic information to screen
    printDebugWarnings = true; //Toggle improper use/potentially unintended behavior warnings
    //Initialize object fields
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
    
    //initialize the adjacencyList indices
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        for (int j = 0; j < 3; ++j) {
            adjacencyList[i][j] = NOT_SET;
        }
    }
    
    //Initialize the large array of corner combinations
    for (int i = 0; i < 70; ++i) {
        for (int j = 0; j < 4; ++j){
            corners2DPossibleCombinations[i][j] = aiVector2D(0.0f, 0.0f);
        }
    }
}

void AACollisionBox::buildCornerAdjacencyList() { //Note that this function must be called before any rotations occur
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

void AACollisionBox::setCorners3D() {
    corners3D[0] = xAxisMajor + yAxisMajor + zAxisMajor;
    corners3D[1] = xAxisMajor + yAxisMajor + zAxisMinor;
    corners3D[2] = xAxisMajor + yAxisMinor + zAxisMajor;
    corners3D[3] = xAxisMajor + yAxisMinor + zAxisMinor;
    corners3D[4] = xAxisMinor + yAxisMajor + zAxisMajor;
    corners3D[5] = xAxisMinor + yAxisMajor + zAxisMinor;
    corners3D[6] = xAxisMinor + yAxisMinor + zAxisMajor;
    corners3D[7] = xAxisMinor + yAxisMinor + zAxisMinor;
}

void AACollisionBox::doRotationsAndRecalculate() {
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
    setCorners3D(); //Set the 3D corners array to the rotated values
    calculateSelfAfterTranslations();
}

void AACollisionBox::calculateSelfAfterTranslations() {
    //This function updates the values in the corners2D and corners3D arrays
    
    aiVector3D unorderedCornersArray[CUBOID_CORNERS];
    unorderedCornersArray[0] = xAxisMajor + yAxisMajor + zAxisMajor;
    unorderedCornersArray[1] = xAxisMajor + yAxisMajor + zAxisMinor;
    unorderedCornersArray[2] = xAxisMajor + yAxisMinor + zAxisMajor;
    unorderedCornersArray[3] = xAxisMajor + yAxisMinor + zAxisMinor;
    unorderedCornersArray[4] = xAxisMinor + yAxisMajor + zAxisMajor;
    unorderedCornersArray[5] = xAxisMinor + yAxisMajor + zAxisMinor;
    unorderedCornersArray[6] = xAxisMinor + yAxisMinor + zAxisMajor;
    unorderedCornersArray[7] = xAxisMinor + yAxisMinor + zAxisMinor;
    
    //new new way of calculating the four corners
    //Find the 2D centriod of all 8 vectors
    
    aiVector2D point1(unorderedCornersArray[0].x / 8.0f, unorderedCornersArray[0].y / 8.0f);
    aiVector2D point2(unorderedCornersArray[1].x / 8.0f, unorderedCornersArray[1].y / 8.0f);
    aiVector2D point3(unorderedCornersArray[2].x / 8.0f, unorderedCornersArray[2].y / 8.0f);
    aiVector2D point4(unorderedCornersArray[3].x / 8.0f, unorderedCornersArray[3].y / 8.0f);
    aiVector2D point5(unorderedCornersArray[4].x / 8.0f, unorderedCornersArray[4].y / 8.0f);
    aiVector2D point6(unorderedCornersArray[5].x / 8.0f, unorderedCornersArray[5].y / 8.0f);
    aiVector2D point7(unorderedCornersArray[6].x / 8.0f, unorderedCornersArray[6].y / 8.0f);
    aiVector2D point8(unorderedCornersArray[7].x / 8.0f, unorderedCornersArray[7].y / 8.0f);
    
    aiVector2D centriod = point1 + point2 + point3 + point4 + point5 + point6 + point7 + point8;
    
    //for debug:
    int cindx1, cindx2, cindx3, cindx4;
    cindx1 = cindx2 = cindx3 = cindx4 = -1;
    
    aiVector2D centriodToCorners[CUBOID_CORNERS];
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        centriodToCorners[i] = aiVector2D(unorderedCornersArray[i].x, unorderedCornersArray[i].y) - centriod;
    }
    
    for (int i = 0; i < BOX_CORNERS; ++i) {
        corners2D[i] = aiVector2D(0.0f, 0.0f);
    }
    
    //Okay so this works if roll is at -90º, 0º or 90º, but fails all other times
    //Need to find the four largest vectors in each quadrant
    //Loop through the corner array again
    for (int i = 0; i < CUBOID_CORNERS; i+=1) {
        if (centriodToCorners[i].x > 0.0f) {
            if (centriodToCorners[i].y > 0.0f) {
                //Then vector is in quadrant 1
                //Check to see if it is the longest
                if (corners2D[0].Length() < centriodToCorners[i].Length()) {
                    corners2D[0].x = unorderedCornersArray[i].x;
                    corners2D[0].y = unorderedCornersArray[i].y;
                    cindx1 = i;
                    //corners2D[0] = centriodToCorners[i];
//                    centriodToCorners[i].x = 0.0f;
//                    centriodToCorners[i].y = 0.0f;
                }
            }
            else {
                //Then Vector is in quadrant 4
                if (corners2D[1].Length() < centriodToCorners[i].Length()) {
                    corners2D[1].x = unorderedCornersArray[i].x;
                    corners2D[1].y = unorderedCornersArray[i].y;
//                    corners2D[1] = centriodToCorners[i];
//                    centriodToCorners[i].x = 0.0f;
//                    centriodToCorners[i].y = 0.0f;
                    cindx2 = i;
                }
            }
        }
        else { //Else x coord is <=0.0f
            if (centriodToCorners[i].y > 0.0f) {
                //Then vector is in quadrant 2
                //Check to see if it is the longest
                if (corners2D[3].Length() < centriodToCorners[i].Length()) {
                    corners2D[3].x = unorderedCornersArray[i].x;
                    corners2D[3].y = unorderedCornersArray[i].y;
//                    corners2D[3] = centriodToCorners[i];
//                    centriodToCorners[i].x = 0.0f;
//                    centriodToCorners[i].y = 0.0f;
                    cindx4 = i;
                }
            }
            else {
                //Then Vector is in quadrant 3
                if (corners2D[2].Length() < centriodToCorners[i].Length()) {
                    corners2D[2].x = unorderedCornersArray[i].x;
                    corners2D[2].y = unorderedCornersArray[i].y;
//                    corners2D[2] = centriodToCorners[i];
//                    centriodToCorners[i].x = 0.0f;
//                    centriodToCorners[i].y = 0.0f;
                }
                cindx3 = i;
            }
        }
    }
    
    //AFter running this check, it looks like the problem is occuring when 0 and 2 are getting set to the same value or 1 and 3 are getting set to the same value. So I am going to do a direct, inefficent fix for this
   //////DEBUG CHECK
   ////Check to see if any of the two corners got set to eachother
    //for (int i = 0; i < BOX_CORNERS; ++i) {
     //   for (int j = i+1; j < BOX_CORNERS; ++j) {
            //if (corners2D[i] == corners2D[j]) {
                //std::cout << "\nCorners[" << i << "] == Corners[" << j << "]\n";
                //std::cout << "Corners[" << i << "] = " << corners2D[i].x << ", " << corners2D[i].y << std::endl;
                //std::cout << "Corners[" << j << "] = " << corners2D[j].x << ", " << corners2D[j].y << std::endl;
                //corners2D[j].x *= -1.0f;
                //corners2D[j].y *= -1.0f;
           // }
        //}
    //}
    //Check for pathological case where there are no vectors in 2 of the 4 quadrants
    if (corners2D[0] == corners2D[2]) { //If two corners were set to the same value (chances are both are still 0 vector)
        //Then assign them to the second longest corner in the other two quadrants (note that 1 is quadrant 4 and 3 is quadrant 2)  (confusing, I know...)
        //Move in order, set corner 0 to second largest vector in corner 1's quadrant, and set corner 2 to second longest vec in corner 3's quadrant
        
        //Do special process for corner 0:
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (unorderedCornersArray[i].x == corners2D[1].x && unorderedCornersArray[i].y == corners2D[1].y) {
                //Delete the longest vector from corner 1's quadrant
                unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
            }
        } //Set corner 0 to the second longest vector in corner 1's quadrant
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (centriodToCorners[i].x > 0.0f) {
                if (centriodToCorners[i].y < 0.0f) {
                    if (corners2D[0].Length() < centriodToCorners[i].Length()) {
                        corners2D[0].x = unorderedCornersArray[i].x;
                        corners2D[0].y = unorderedCornersArray[i].y;
                        cindx1 = i;
                    }
                }
            }
        }
        
        //Repeat similar special process for corner 2:
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (unorderedCornersArray[i].x == corners2D[3].x && unorderedCornersArray[i].y == corners2D[3].y) {
                //Delete the longest vector from corner 3's quadrant
                unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
            }
        } //Set corner 2 to the second longest vector in corner 3's quadrant
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (centriodToCorners[i].x < 0.0f) {
                if (centriodToCorners[i].y > 0.0f) {
                    if (corners2D[2].Length() < centriodToCorners[i].Length()) {
                        corners2D[2].x = unorderedCornersArray[i].x;
                        corners2D[2].y = unorderedCornersArray[i].y;
                        cindx3 = i;
                    }
                }
            }
        }
        /*if (corners2D[0].x > 0.0f) {
            corners2D[2] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
            //loop through the array of unordered corners
            for (int i = 0; i < CUBOID_CORNERS; ++i) {
                if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
                }
            }
            for (int i = 0; i < CUBOID_CORNERS; ++i) {
                if (centriodToCorners[i].x > 0.0f) {
                    if (centriodToCorners[i].y > 0.0f) {
                        if (corners2D[2].Length() < centriodToCorners[i].Length()) {
                            corners2D[2].x = unorderedCornersArray[i].x;
                            corners2D[2].y = unorderedCornersArray[i].y;
                        }
                    }
                }
            }
        }
        else {
            corners2D[0] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
            //loop through the array of unordered corners
            for (int i = 0; i < CUBOID_CORNERS; ++i) {
                if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
                }
            }
            for (int i = 0; i < CUBOID_CORNERS; ++i) {
                if (centriodToCorners[i].x < 0.0f) {
                    if (centriodToCorners[i].y > 0.0f) {
                        if (corners2D[0].Length() < centriodToCorners[i].Length()) {
                            corners2D[0].x = unorderedCornersArray[i].x;
                            corners2D[0].y = unorderedCornersArray[i].y;
                        }
                    }
                }
            }
        } */
    }
    
    
    
    //This code below right here fixes the wrong problem. If two vectors are equal, it's because no longest vector existed within their quadrant
//    //This code will fix faulty boxes formed if two vectors are in the same quadrant
//    if (corners2D[0] == corners2D[2]) { //If two corners were set to the same value
//        //Figure out what quadrant this vector exists in (NOTE THAT FOR 0 and 2, Y will always be positive)
//        if (corners2D[0].x > 0.0f) {
//            corners2D[2] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
//            //loop through the array of unordered corners
//            for (int i = 0; i < CUBOID_CORNERS; ++i) {
//                if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
//                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//                }
//            }
//            for (int i = 0; i < CUBOID_CORNERS; ++i) {
//                if (centriodToCorners[i].x > 0.0f) {
//                    if (centriodToCorners[i].y > 0.0f) {
//                        if (corners2D[2].Length() < centriodToCorners[i].Length()) {
//                            corners2D[2].x = unorderedCornersArray[i].x;
//                            corners2D[2].y = unorderedCornersArray[i].y;
//                        }
//                    }
//                }
//            }
//        }
//        else {
//            corners2D[0] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
//            //loop through the array of unordered corners
//            for (int i = 0; i < CUBOID_CORNERS; ++i) {
//                if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
//                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//                }
//            }
//            for (int i = 0; i < CUBOID_CORNERS; ++i) {
//                if (centriodToCorners[i].x < 0.0f) {
//                    if (centriodToCorners[i].y > 0.0f) {
//                        if (corners2D[0].Length() < centriodToCorners[i].Length()) {
//                            corners2D[0].x = unorderedCornersArray[i].x;
//                            corners2D[0].y = unorderedCornersArray[i].y;
//                        }
//                    }
//                }
//            }
//        }
//    }
    
    
    if (corners2D[1] == corners2D[3]) { //If other two corners were set to the same value (chances are both are still 0 vector)
        
        //Do special process for corner 1:
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
                //Delete the longest vector from corner 1's quadrant
                unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
            }
        } //Set corner 1 to the second longest vector in corner 2's quadrant
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (centriodToCorners[i].x < 0.0f) {
                if (centriodToCorners[i].y < 0.0f) {
                    if (corners2D[1].Length() < centriodToCorners[i].Length()) {
                        corners2D[1].x = unorderedCornersArray[i].x;
                        corners2D[1].y = unorderedCornersArray[i].y;
                        cindx2 = i;
                    }
                }
            }
        }
        
        //Repeat similar special process for corner 3:
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
                //Delete the longest vector from corner 3's quadrant
                unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
                centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
            }
        } //Set corner 3 to the second longest vector in corner 0's quadrant
        for (int i = 0; i < CUBOID_CORNERS; ++i) {
            if (centriodToCorners[i].x > 0.0f) {
                if (centriodToCorners[i].y > 0.0f) {
                    if (corners2D[3].Length() < centriodToCorners[i].Length()) {
                        corners2D[3].x = unorderedCornersArray[i].x;
                        corners2D[3].y = unorderedCornersArray[i].y;
                        cindx4 = i;
                    }
                }
            }
        }
    }
    
    //Need to now scale corners2D and put them around the objects midpoint
    corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[0].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[0].y)));
    corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[3].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[3].y)));
    corners2D[2] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[2].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[2].y)));
    corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[1].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[1].y)));
    
    
    //Print the corner indexes that were selected
    std::cout << "\nCorners from the 8 corners that were selected (indexed from 0): " << cindx1 << " " << cindx2 << " " << cindx3 << " " << cindx4 << std::endl;
    
    
    /*  This new way is really close, but not quite
    //New Different way of getting the four corners
    //Set all vectors in corners2D to 0
    for (int i = 0; i < BOX_CORNERS; ++i) {
        corners2D[i].x = 0.0f;
        corners2D[i].y = 0.0f;
    }
    
    aiVector2D cornersProjectedOntoPlane[CUBOID_CORNERS];
    for (int i = 0; i < CUBOID_CORNERS; i+=1) {
        cornersProjectedOntoPlane[i].x = unorderedCornersArray[i].x;
        cornersProjectedOntoPlane[i].y = unorderedCornersArray[i].y;
    }
    //Need to find the four largest vectors in each quadrant
    //Loop through the corner array again
    for (int i = 0; i < CUBOID_CORNERS; i+=1) {
        if (cornersProjectedOntoPlane[i].x > 0.0f) {
            if (cornersProjectedOntoPlane[i].y > 0.0f) {
                //Then vector is in quadrant 1
                //Check to see if it is the longest
                if (corners2D[0].Length() < cornersProjectedOntoPlane[i].Length()) {
                    corners2D[0] = cornersProjectedOntoPlane[i];
                }
            }
            else {
                //Then Vector is in quadrant 4
                if (corners2D[1].Length() < cornersProjectedOntoPlane[i].Length()) {
                    corners2D[1] = cornersProjectedOntoPlane[i];
                }
            }
        }
        else { //Else x coord is <=0.0f
            if (cornersProjectedOntoPlane[i].y > 0.0f) {
                //Then vector is in quadrant 2
                //Check to see if it is the longest
                if (corners2D[3].Length() < cornersProjectedOntoPlane[i].Length()) {
                    corners2D[3] = cornersProjectedOntoPlane[i];
                }
            }
            else {
                //Then Vector is in quadrant 3
                if (corners2D[2].Length() < cornersProjectedOntoPlane[i].Length()) {
                    corners2D[2] = cornersProjectedOntoPlane[i];
                }
            }
        }
    }
    
    //Need to now scale corners2D and put them around the objects midpoint
    corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[0].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[0].y)));
     corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[3].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[3].y)));
     corners2D[2] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[2].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[2].y)));
     corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[1].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[1].y)));
    
    */
    
    
    /*
    //I think my collision box is being bound to the axeses here, so need to find
    //a diffrernt way to construct the 4 extreme points
    //Need to get the four points that represent the extremes along x and y axes
    int largestXIndxPos, largestXIndxNeg, largestYIndxPos, largestYIndxNeg;
    largestXIndxPos = largestXIndxNeg = largestYIndxPos = largestYIndxNeg = 0;
    
    //Now loop through the array to find which vectors in the cornerArray contain these maximums
    float maxXPos, maxXNeg, maxYPos, maxYNeg;
    maxXPos = maxXNeg = maxYPos = maxYNeg = 0.0f;
    for (int i = 0; i < CUBOID_CORNERS; ++i) {
        //get most positive x
        if (unorderedCornersArray[i].x > maxXPos) {
            maxXPos = unorderedCornersArray[i].x;
            largestXIndxPos = i;
        }
        //get most negative x
        if (unorderedCornersArray[i].x < maxXNeg) {
            maxXNeg = unorderedCornersArray[i].x;
            largestXIndxNeg = i;
        }
        //get most positive y
        if (unorderedCornersArray[i].y > maxYPos) {
            maxYPos = unorderedCornersArray[i].y;
            largestYIndxPos = i;
        }
        //get most negative y
        if (unorderedCornersArray[i].y < maxYNeg) {
            maxYNeg = unorderedCornersArray[i].y;
            largestYIndxNeg = i;
        }
        this->corners3D[i] = unorderedCornersArray[i];
    }
    
    //Set corners2D array
    corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxPos].x)), scale * (midpoint.y - (collisionBoxShrinkageFactor * corners3D[largestYIndxPos].y)));
    corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y - (collisionBoxShrinkageFactor * corners3D[largestYIndxPos].y)));
    corners2D[2] = aiVector2D(scale * (midpoint.x - (collisionBoxShrinkageFactor * corners3D[largestXIndxPos].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
    corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
    
   // corner1 = aiVector2D(scale*(midpoint.x + (boxShrinkageFactor * maxXPos)), scale *  (midpoint.y - (boxShrinkageFactor * maxYPos)));
     //   corner2 = aiVector2D(scale * (midpoint.x + (boxShrinkageFactor * maxXNeg)), scale * (midpoint.y - (boxShrinkageFactor * maxYNeg))); */
    
}


bool AACollisionBox::hasNoArea() const {
    if (!hasModelData) {return true;}
    //Else do a calculation to see if the box actually has no area for some reason
    //The calculation is to test all four points, to see if they all lie along the same line
    
    //Math note: This can be used to determine if three points are colinear (might be overkill?):
    //                http://mathworld.wolfram.com/Cayley-MengerDeterminant.html
    
    return (getQuadrilateralArea() == 0);
}

float AACollisionBox::getQuadrilateralArea() const{
    //Think I fixed the bug so going to temporarily remove this warning (for now) until I can test
    //std::cout << "\nDEBUG::Warning! Get Quadrilateral area is buggy and won't work on all quadrilaterals. Fix this function\n";
    //This isn't going to work until I can guarentee the ordering of the corners
    float x0 = corners2D[0].x;          //It is assumed that corners2D[0] and corners2D[2] are opposite sides (not connected)
    float y0 = corners2D[0].y;          //Same with corners2D[1] and corners2D[3]
    float x1 = corners2D[1].x;
    float y1 = corners2D[1].y;
    float x2 = corners2D[2].x;
    float y2 = corners2D[2].y;
    float x3 = corners2D[3].x;
    float y3 = corners2D[3].y;
    
    //Could use the Cayley-Menger determinant, or could just be simple and calculate the area of two triangles and add
    //(technically these next 2 computations are themselves determinants) see: http://mathworld.wolfram.com/TriangleArea.html
    float triangle1Area = (-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2); //Note this is double the triangle area
    float triangle2Area = (-x3*y0 + x2*y0 + x0*y3 - x2*y3 - x0*y2 + x3*y2); //I will just wait till end to divide by 2
    return ((abs(triangle1Area) + abs(triangle2Area)) / 2.0f);
}


/*!
This Code is for finding the largest absolute value in a collection of floats.
@remark This just returns the maximum value in an array of floats using absolue value
*/
float AACollisionBox::getMaxFromArray(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (abs(data[i]) > max) {
            max = abs(data[i]);
        }
    }
    return max;
}

float AACollisionBox::getMaxFromArrayPositiveOnly(float * data, int dataSize) {
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
float AACollisionBox::getMaxFromArrayNegativeOnly(float * data, int dataSize) {
    float negMax = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (data[i] < negMax) {
            negMax = data[i];
        }
    }
    return negMax;
}



