//
//  AACollisionBox.cpp
//
//  Created by Forrest Miller on 3/5/18.
//

#include "AACollisionBox.h"

static const float STEP_SIZE = 0.01f; //Used for seperating two overlapping CollisionBoxes

static const int CUBOID_CORNERS = 8;
static const int BOX_CORNERS = 4;


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
    
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, dataPoints / 2);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, dataPoints / 2);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, dataPoints / 2);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, dataPoints / 2);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, dataPoints / 2);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, dataPoints / 2);
    
    
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
    
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, dataPoints / 2);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, dataPoints / 2);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, dataPoints / 2);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, dataPoints / 2);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, dataPoints / 2);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, dataPoints / 2);
    
    
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
    bufferOfTwelveFloats[2] = 0.5f;
    
    bufferOfTwelveFloats[3] = this->corners2D[1].x;
    bufferOfTwelveFloats[4] = this->corners2D[1].y;
    bufferOfTwelveFloats[5] = 0.5f;
    
    bufferOfTwelveFloats[6] = this->corners2D[2].x;
    bufferOfTwelveFloats[7] = this->corners2D[2].y;
    bufferOfTwelveFloats[8] = 0.5f;
    
    bufferOfTwelveFloats[9] = this->corners2D[3].x;
    bufferOfTwelveFloats[10] = this->corners2D[3].y;
    bufferOfTwelveFloats[11] = 0.5f;
}
void AACollisionBox::getRect3DCornerPoints3D(float * bufferOfTwentyfourFloats) const {
    std::cout << "\nDEBUG::Warning! getRect3DCornerPoints3D() has not yet been implemented!\n";
}

//These next two return formated position values for drawing various primatives
void AACollisionBox::getRectCornersTriangles3D(float * bufferOfEighteenFloats) const {
    //Triangle 1
    bufferOfEighteenFloats[0] = this->corners2D[0].x;
    bufferOfEighteenFloats[1] = this->corners2D[0].y;
    bufferOfEighteenFloats[2] = 0.5f;
    bufferOfEighteenFloats[3] = this->corners2D[1].x;
    bufferOfEighteenFloats[4] = this->corners2D[1].y;
    bufferOfEighteenFloats[5] = 0.5f;
    bufferOfEighteenFloats[6] = this->corners2D[2].x;
    bufferOfEighteenFloats[7] = this->corners2D[2].y;
    bufferOfEighteenFloats[8] = 0.5f;
    //Triangle 2
    bufferOfEighteenFloats[9] = this->corners2D[2].x;
    bufferOfEighteenFloats[10] = this->corners2D[2].y;
    bufferOfEighteenFloats[11] = 0.5f;
    bufferOfEighteenFloats[12] = this->corners2D[3].x;
    bufferOfEighteenFloats[13] = this->corners2D[3].y;
    bufferOfEighteenFloats[14] = 0.5f;
    bufferOfEighteenFloats[15] = this->corners2D[0].x;
    bufferOfEighteenFloats[16] = this->corners2D[0].y;
    bufferOfEighteenFloats[17] = 0.5f;
}
void AACollisionBox::getRectCornersLines3D(float * bufferOfTwentyfourFloats) const {
    bufferOfTwentyfourFloats[0] = this->corners2D[0].x;
    bufferOfTwentyfourFloats[1] = this->corners2D[0].y;
    bufferOfTwentyfourFloats[2] = 0.5f;
    bufferOfTwentyfourFloats[3] = this->corners2D[1].x;
    bufferOfTwentyfourFloats[4] = this->corners2D[1].y;
    bufferOfTwentyfourFloats[5] = 0.5f;
    
    bufferOfTwentyfourFloats[6] = this->corners2D[1].x;
    bufferOfTwentyfourFloats[7] = this->corners2D[1].y;
    bufferOfTwentyfourFloats[8] = 0.5f;
    bufferOfTwentyfourFloats[9] = this->corners2D[2].x;
    bufferOfTwentyfourFloats[10] = this->corners2D[2].y;
    bufferOfTwentyfourFloats[11] = 0.5f;
    
    bufferOfTwentyfourFloats[12] = this->corners2D[2].x;
    bufferOfTwentyfourFloats[13] = this->corners2D[2].y;
    bufferOfTwentyfourFloats[14] = 0.5f;
    bufferOfTwentyfourFloats[15] = this->corners2D[3].x;
    bufferOfTwentyfourFloats[16] = this->corners2D[3].y;
    bufferOfTwentyfourFloats[17] = 0.5f;
    
    bufferOfTwentyfourFloats[18] = this->corners2D[3].x;
    bufferOfTwentyfourFloats[19] = this->corners2D[3].y;
    bufferOfTwentyfourFloats[20] = 0.5f;
    bufferOfTwentyfourFloats[21] = this->corners2D[0].x;
    bufferOfTwentyfourFloats[22] = this->corners2D[0].y;
    bufferOfTwentyfourFloats[23] = 0.5f;
    
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
    calculateSelfAfterTranslations();
}

void AACollisionBox::calculateSelfAfterTranslations() {
    //This function updates the values in the corners2D and corners3D arrays
    
    aiVector3D unorderedCornersArray[8];
    unorderedCornersArray[0] = xAxisMajor + yAxisMajor + zAxisMajor;
    unorderedCornersArray[1] = xAxisMajor + yAxisMajor + zAxisMinor;
    unorderedCornersArray[2] = xAxisMajor + yAxisMinor + zAxisMajor;
    unorderedCornersArray[3] = xAxisMajor + yAxisMinor + zAxisMinor;
    unorderedCornersArray[4] = xAxisMinor + yAxisMajor + zAxisMajor;
    unorderedCornersArray[5] = xAxisMinor + yAxisMajor + zAxisMinor;
    unorderedCornersArray[6] = xAxisMinor + yAxisMinor + zAxisMajor;
    unorderedCornersArray[7] = xAxisMinor + yAxisMinor + zAxisMinor;
    
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
    corners2D[1] = aiVector2D(scale * (midpoint.x - (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y - (collisionBoxShrinkageFactor * corners3D[largestYIndxPos].y)));
    corners2D[2] = aiVector2D(scale * (midpoint.x - (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
    corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxPos].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
    
   // corner1 = aiVector2D(scale*(midpoint.x + (boxShrinkageFactor * maxXPos)), scale *  (midpoint.y - (boxShrinkageFactor * maxYPos)));
     //   corner2 = aiVector2D(scale * (midpoint.x + (boxShrinkageFactor * maxXNeg)), scale * (midpoint.y - (boxShrinkageFactor * maxYNeg)));
    
    
    
//    // //-----------------THIS NEXT PART COULD BE BUGGY IF MY MATH IS WRONG!--------------  (so close!)
//    //Need to get the four points that represent the extremes along x and y axes
//    int largestXIndxPos, largestXIndxNeg, largestYIndxPos, largestYIndxNeg;
//    largestXIndxPos = largestXIndxNeg = largestYIndxPos = largestYIndxNeg = 0;
//
//    //Now loop through the array to find which vectors in the cornerArray contain these maximums
//    float maxXPos, maxXNeg, maxYPos, maxYNeg;
//    maxXPos = maxXNeg = maxYPos = maxYNeg = 0.0f;
//    for (int i = 0; i < 8; ++i) {
//        //get largest x
//        if (cornerArray[i].x > maxXPos) {
//            maxXPos = cornerArray[i].x;
//            largestXIndxPos = i;
//        }
//        //get most negative x
//        if (cornerArray[i].x < maxXNeg) {
//            maxXNeg = cornerArray[i].x;
//            largestXIndxNeg = i;
//        }
//        //get largest y
//        if (cornerArray[i].y > maxYPos) {
//            maxYPos = cornerArray[i].y;
//            largestYIndxPos = i;
//        }
//        //get most negative y
//        if (cornerArray[i].y < maxYNeg) {
//            maxYNeg = cornerArray[i].y;
//            largestYIndxNeg = i;
//        }
//        this->corners[i] = cornerArray[i];
//        //std::cout << "\nCorner[" << i << "] = " << corners[i].x << ", " << corners[i].y << ", " << corners[i].z;
//    }
//
//    float boxShrinkageFactor = collisionBoxShrinkageFactor; //0.85f-0.90f seem like good values for this
//
//    corner1 = aiVector2D(scale*(midpoint.x + (boxShrinkageFactor * maxXPos)), scale *  (midpoint.y - (boxShrinkageFactor * maxYPos)));
//    corner2 = aiVector2D(scale * (midpoint.x + (boxShrinkageFactor * maxXNeg)), scale * (midpoint.y - (boxShrinkageFactor * maxYNeg)));
    
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


