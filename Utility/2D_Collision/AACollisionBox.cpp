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
//FUDGE_FACTOR is used to make there be a max/min x-y value in case there is a tie when generating 2D box
static const float FUDGE_FACTOR = 0.00001f; //FUDGE_FACTOR should be at least an order of magnitude less than the
                                            //smallest rotation change amount
//              NOTE THAT FUDGE FACTOR OF 0.000001f causes a crash because value is too small

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
    
    buildCornerAdjacencyList();
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
void AACollisionBox::changeRotationAt(int index, float theta) {
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
void AACollisionBox::changeRotitationAxisAt(int index, const Quaternion & rQuat) {
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
void AACollisionBox::changeRotitationAxisAt(int index, const aiVector3D & axis, float theta) {
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
void AACollisionBox::changeRotitationAxisAt(int index, float x, float y, float z, float theta) {
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
void AACollisionBox::removeRotationAtIndex(int index) {
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

bool AACollisionBox::isWithin(float x, float y) const {
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

bool AACollisionBox::isWithin(const aiVector2D & point) const {
    if (!hasModelData || hasNoArea()) {return false;}
    //See the other version of this function for comments on computation being performed
    float triangleAreaSum = getTriangleArea(corners2D[0], point, corners2D[1]) +
                            getTriangleArea(corners2D[1], point, corners2D[2]) +
                            getTriangleArea(corners2D[2], point, corners2D[3]) +
                            getTriangleArea(corners2D[3], point, corners2D[0]);
    return (abs(triangleAreaSum - this->getQuadrilateralArea()) <= 0.00005f); //Just some floating point tolerance in case result is not exactly 0
}

bool AACollisionBox::isOverlapping(const AACollisionBox& otherBox) const {
    if (!hasModelData || !(otherBox.hasModelData)) {
        return false;
    }
    //There are several pathological cases I have drawn out on paper (most involving boxes of vastly different size),
    //so I am going to try to get a very general algorithm for checking.
    
    //Idea: It will be easier to do some quick checks to rule out cases where it isn't possible for the two rectangles to be overlapping
    
    //Check to see if all four points of one rectangle lie beyond the max/min of the other
    //Start with all points of other having x coords greater than this rectangles x0
    bool allPointsOfOtherBoxBeyondThisBoxExtrema = true;
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.corners2D[i].x <= corners2D[0].x) {
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
    
    //I haven't definitivly proved it, but I am pretty sure that combining that last check with
    //this next one will always determine if there is any overlap
    
    //Check to see if any of the four corners from one box are inside the other
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (otherBox.isWithin(this->corners2D[i])) {
            return true;
        }
    }
    //Also then do the same check with otherBox's corners inside this one
    for (int i = 0; i < BOX_CORNERS; ++i) {
        if (isWithin(otherBox.corners2D[i])) {
            return true;
        }
    }
    return false; //If both tests passed without returning, then boxes are not overlapping
}

void AACollisionBox::moveApartAlongAxisBetweenClosestDetectedPoints(AACollisionBox & other) {
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

void AACollisionBox::moveApartAlongAxisBetweenMidpoints(AACollisionBox & otherBox) {
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
    } while (this->isOverlapping(otherBox));
}


void AACollisionBox::moveApartAlongAxisBetweenMidpointsThisOnly(AACollisionBox & otherBox) {
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

void AACollisionBox::moveApartAlongAxisBetweenMidpointsOtherOnly(AACollisionBox & otherBox) {
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
        //Move the other rectangle away also by step amount
        otherBox.midpoint -= displacement * STEP_SIZE;
        otherBox.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherBox));
}
//------------------------------------------------------------------------
//              Getters for drawing AACollisionBox
//------------------------------------------------------------------------

void AACollisionBox::getRotatedMajorMinor3D(float * bufferOfThirtysixFloats) const {
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

//DEBUG::This one tested and it works
void AACollisionBox::getCubiodTriangles3D(float * bufferOf108Floats) const {
    aiVector3D modelTranslantedCorners3D[8];
    aiVector3D midpoint3D = aiVector3D(midpoint.x, midpoint.y, 0.0f);
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

//TESTED THIS ONE AND IT WORKS
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

void AACollisionBox::resetCollisionDetectionSamplePoints() {
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

void AACollisionBox::getCollisionDetectionSamplePointsBoxMidpointToBoxMidpoint(float * bufferOfSixFloats, const AACollisionBox &otherBox) const {
    bufferOfSixFloats[0] = otherBox.midpoint.x;
    bufferOfSixFloats[1] = otherBox.midpoint.y;
    bufferOfSixFloats[2] = -0.75f;
    bufferOfSixFloats[3] = this->midpoint.x;
    bufferOfSixFloats[4] = this->midpoint.y;
    bufferOfSixFloats[5] = -0.75f;
}

std::vector<float>* AACollisionBox::getCollisionDetectionSamplePoints() const {
    if (collisionSamplePointsAll == nullptr) {
        if (printDebugWarnings) {
            std::cout << "\nDEBUG::WARNING collisionSamplePointsAll is null for this collision box!\n";
        }
        return nullptr;
    }
    return collisionSamplePointsAll;
}

std::vector<float>* AACollisionBox::getCollisionDetectionSamplePointsThisOnly() const {
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

void AACollisionBox::calculateSelfAfterTranslations() {
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
        //I could write a seperate algorithm to handle the case of the box being axis align with the basis vectors,
        //or I could just do this:
        //Add a hidden rotation
        useHiddenRotation = true;
        doRotationsAndRecalculate(); //Redo the 2D box formation process with the hidden rotation included
    }
}

bool AACollisionBox::hasNoArea() const {
    if (!hasModelData) {return true;}
    //Math note: This can be used to determine if three points are colinear (might be overkill?):
    //                http://mathworld.wolfram.com/Cayley-MengerDeterminant.html
    return (abs(getQuadrilateralArea()) <= 0.000001f); //See if the quad area is (nearly) 0.0ƒ
}

float AACollisionBox::getQuadrilateralArea() const {
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



