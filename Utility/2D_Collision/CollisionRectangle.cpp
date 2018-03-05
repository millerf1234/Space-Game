//
//  CollisionRectangle.cpp
//
//  Created by Forrest Miller on 2/28/18.
//

#include "CollisionRectangle.h"

static constexpr float STEP_SIZE = 0.01f; //Used for determining how fast to move two collosionRectangles apart

//Note that these 2 booleans are independent, so having either on will result in messages being printed
static constexpr bool DEBUG_MSG_ON = true; //Turn this off to stop debug messages from being printed
static constexpr bool DEBUG_WARNING_MSG_ON = true; //Turn this off to stop debug warning messages from being printed

//------------------------------------------------------------------------
//              CONSTRUCTORS
//------------------------------------------------------------------------
CollisionRectangle::CollisionRectangle() {
    xMajorMinorEqual = yMajorMinorEqual = zMajorMinorEqual = false; //Initialize these to false
    xAxisMajor = xAxisMinor = yAxisMajor = yAxisMinor = zAxisMajor = zAxisMinor = aiVector3D(0.0f, 0.0f, 0.0f);
    originalMajorsFromModel = originalMinorsFromModel = aiVector3D(0.0f, 0.0f, 0.0f);
    
    //x = y = z = 0.0f;
    //maxFromModelXYZ = aiVector3D(x,y,z);
    
    corner1 = corner2 = aiVector2D(0.0f, 0.0f);
    midpoint = aiVector2D(0.0f, 0.0f);
    rotationOrder = nullptr;
    numberOfRotations = 0;
    rotationOrderSize = 0;
    hasModelData = false;
    scale = 1.0f;
    //calculateSelfBeforeTransformations(); //Don't need to calculate self if everything is 0? Investigate when I get time
}

/*!
 Constructor that initializes the collisionRectangle straight from the model data. If model has no z component, just
 create an array with x and y coordinates as desired and set every third value to 0.0f.
 @remark  It is assumed in this constructor that Data is model data in the form x0,y0,z0,x1,y1,z1,x2,y2,z2,...
 */
CollisionRectangle::CollisionRectangle(float * data, int dataPoints) {
    if (dataPoints % 3 != 0) {
        if (DEBUG_MSG_ON || DEBUG_WARNING_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! Attempting to construct a Collision Rectangle from\ndata that is not a multiple of 3!!";
            std::cout << std::endl;
        } //Throw an exception? Maybe later once I have more classes written that can share exceptions to be thrown
        //(*this) = CollisionRectangle(); //Just call the parameterless constructor
        ////Check to make sure the array stays in bounds when checking bounds
        //if (dataPoints % 3 != 0) { //dataPoints must be divisible by 3
        //    if (DEBUG_MSG_ON) {
        //        std::cout << "\nDEBUG::Warning! You are trying to construct a collisionRectangle with an";
        //        std::cout << " array of data who's size is not divisible by 3.\n";
        //        std::cout << "Bounding Box was not set from model data!" << std::endl;
        //    }
        //    return;
        //}
        xMajorMinorEqual = yMajorMinorEqual = zMajorMinorEqual = false; //Initialize these to false
        xAxisMajor = xAxisMinor = yAxisMajor = yAxisMinor = zAxisMajor = zAxisMinor = aiVector3D(0.0f, 0.0f, 0.0f);
        originalMajorsFromModel = originalMinorsFromModel = aiVector3D(0.0f, 0.0f, 0.0f);
        //x = y = z = 0.0f;
        //maxFromModelXYZ = aiVector3D(x,y,z);
        corner1 = corner2 = aiVector2D(0.0f, 0.0f);
        midpoint = aiVector2D(0.0f, 0.0f);
        rotationOrder = nullptr;
        numberOfRotations = 0;
        rotationOrderSize = 0;
        scale = 1.0f;
        hasModelData = false;
        return;
    }
    //Initialize values
    midpoint = aiVector2D(0.0f, 0.0f);
    rotationOrder = nullptr;
    numberOfRotations = 0;
    rotationOrderSize = 0;
    
    scale = 1.0f;
    
    //set rotatedXYZ
    float xDataPoints[dataPoints / 2]; //Give it some extra space, just in case
    float yDataPoints[dataPoints / 2]; //Same deal, extra space for just in case
    float zDataPoints[dataPoints / 2]; //Ya know the deal by now
    
    
    //Set all values in the three array to 0.0f so result of max calculation won't be incorrect
    //Is this faster or is doing all 3 in the same loop?
    for (int i = 0; i < dataPoints / 2; ++i) {
        xDataPoints[i] = 0.0f;
    }
    for (int j = 0; j < dataPoints / 2; ++j) {
        yDataPoints[j] = 0.0f;
    }
    for (int k = 0; k < dataPoints / 2; ++k) {
        zDataPoints[k] = 0.0f;
    }
    //potentially faster method (but this also could potentially be slower):
    //for (int i = 0; i < dataPoints / 2; ++i) {
    //    xDataPoints = yDataPoints = zDataPoints = 0.0f;
    //}
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
    
    //Get both the maximum positive and negative values, for six total
    float maxPosX, maxNegX, maxPosY, maxNegY, maxPosZ, maxNegZ;
    maxPosX = maxNegX = maxPosY = maxNegY = maxPosZ = maxNegZ = 0.0f; //Initialize all 6 to 0.
    //Set the maximums from model data:
    maxPosX = getMaxFromArrayPositiveOnly(xDataPoints, dataPoints / 2);
    maxNegX = getMaxFromArrayNegativeOnly(xDataPoints, dataPoints / 2);
    maxPosY = getMaxFromArrayPositiveOnly(yDataPoints, dataPoints / 2);
    maxNegY = getMaxFromArrayNegativeOnly(yDataPoints, dataPoints / 2);
    maxPosZ = getMaxFromArrayPositiveOnly(zDataPoints, dataPoints / 2);
    maxNegZ = getMaxFromArrayNegativeOnly(zDataPoints, dataPoints / 2);
    
    //Figure out which direction along each basis axis will be major and which will be minor
    //Along the x axis:
    if (maxPosX == abs(maxNegX)) {
        xMajorMinorEqual = true;
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
    }
    else if (maxPosX > abs(maxNegX)) { //Else if positive x values have larger max then neg x values
        xMajorMinorEqual = false; //False because one axis was bigger than the other
        //use positive as major
        xAxisMajor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxPosX;
        xAxisMinor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxNegX;
    }
    else { //else negative x must have contained overall larger values
        xMajorMinorEqual = false; //False because one axis was bigger than the other
        //use negative as major
        xAxisMajor = aiVector3D(maxNegX, 0.0f, 0.0f);
        originalMajorsFromModel.x = maxNegX;
        xAxisMinor = aiVector3D(maxPosX, 0.0f, 0.0f);
        originalMinorsFromModel.x = maxPosX;
    }
    
    //Along the y axis:
    if (maxPosY == abs(maxNegY)) {
        yMajorMinorEqual = true;
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
    }
    else if (maxPosY > abs(maxNegY)) { //Else if positive Y values have larger max then negative Y values
        yMajorMinorEqual = false; //False because one axis was bigger than the other
        //use positive as major
        yAxisMajor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMajorsFromModel.y = maxPosY;
        yAxisMinor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMinorsFromModel.y = maxNegY;
    }
    else { //else negative y must have contained overall larger values
        yMajorMinorEqual = false; //False because one axis was bigger than the other
        //use negative as major
        yAxisMajor = aiVector3D(0.0f, maxNegY, 0.0f);
        originalMajorsFromModel.y = maxNegY;
        yAxisMinor = aiVector3D(0.0f, maxPosY, 0.0f);
        originalMinorsFromModel.y = maxPosY;
    }
    
    //Along the z axis:
    if (maxPosZ == abs(maxNegZ)) { //If the largest distance is equal in both directions
        zMajorMinorEqual = true;
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
    }
    else if (maxPosZ > abs(maxNegZ)) { //Else if positive Z values have larger max then negative Z values
        zMajorMinorEqual = false; //False because one axis was bigger than the other
        //use positive as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMajorsFromModel.z = maxPosZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMinorsFromModel.z = maxNegZ;
    }
    else { //else negative z must have contained overall larger values
        zMajorMinorEqual = false; //False because one axis was bigger than the other
        //use negative as major
        zAxisMajor = aiVector3D(0.0f, 0.0f, maxNegZ);
        originalMajorsFromModel.z = maxNegZ;
        zAxisMinor = aiVector3D(0.0f, 0.0f, maxPosZ);
        originalMinorsFromModel.z = maxPosZ;
    }
    this->hasModelData = true;
    
//    //Here is what I used to do when I would only get the largest absolute value along each axis
//    //Get the max values along each axis of from the model data
//    x = getMaxFromArray(xDataPoints, dataPoints / 2); //Get max x value
//    y = getMaxFromArray(yDataPoints, dataPoints / 2); //Get max y value
//    z = getMaxFromArray(zDataPoints, dataPoints / 2); //Get max z value
//
//    this->maxFromModelXYZ = aiVector3D(x,y,z); //Set this vector from the calculated maximums
    //this->hasModelData = true; //Set to true once bounds have been calculated from model data
    //
    //if (DEBUG_MSG_ON) {
    //    std::cout << "\nDEBUG::CollisionRectangle Size Values Determined!\nxMax: " << x;
    //    std::cout << "\nyMax: " << y << "\nzMax: " << z << std::endl;
   // }
    
   // calculateSelfBeforeTransformations(); //Not sure if I even need this first private func
    doRotationsAndRecalculate();
}

CollisionRectangle::CollisionRectangle(aiVector3D *, int numberOfVectors){
    //Not Yet implemented. Shouldn't be very difficult to implement
    if (DEBUG_MSG_ON || DEBUG_WARNING_MSG_ON) {
        std::cout << "\nDEBUG::OOPS! THIS CONSTRUCTOR FOR COLLISION BOX HASN'T BEEN IMPLEMENTED YET!\n";
    }
    //calculateSelfBeforeTransformations();
    return;
}

//------------------------------------------------------------------------
//              DESTRUCTORS
//------------------------------------------------------------------------
CollisionRectangle::~CollisionRectangle() {
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
void CollisionRectangle::setFromModelData(float * data, int dataPoints) {
    bool dataWasNotTheRightSize = false;
    if (dataPoints % 3 != 0) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! setFromModelData called on this ";
            std::cout << "CollisionRectangle but the\ndata provided to this has";
            std::cout << " a size other than a multiple of 3!\n";
            std::cout << "CollisionRectangle was not set from provided data!\n";
        }
        dataWasNotTheRightSize = true;
    }
    
    //Check to see if this object already has model data
    if (hasModelData) { //if object already had model data
        if (dataWasNotTheRightSize == false) { //and if newly provided data is divisble by 3
            if (DEBUG_MSG_ON) {
                std::cout << "\nDEBUG::ATTENTION! This CollisionRectangle already";
                std::cout << " had model data set!\n";
                std::cout << "CollsisionRectangle will now reconfigure itself from ";
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
            if (DEBUG_MSG_ON || DEBUG_WARNING_MSG_ON) {
                std::cout << "\nDEBUG::OOPS! This collisionRectangle already had";
                std::cout << " model data and the newly\nprovided data to it using";
                std::cout << " the function setFromModelData was not sized a\n";
                std::cout << "multiple of three!\nNew model data not set, so ";
                std::cout << "CollisionRectangle will keep previous data\nit was";
                std::cout << " configured with!\n";
            }
            return; //Return without executing the rest of the function
        }
    } //If there wasn't already model data and provided data was not the right size
    else if (dataWasNotTheRightSize) {
        return; //A debug message for this case was already printed out when
        //the first check for dataWasNotTheRightSize was performed
    }
    
    //Here is where the function actually starts configuring the new CollisionRectangle
    //based off the provided data
    
    std::cout << "\nDEBUG::OOPS! THIS FUNCTION HASN'T BEEN FULLY IMPLEMENTED YET!\n";
    
}


//------------------------------------------------------------------------
//              Rotation Functions
//------------------------------------------------------------------------
void CollisionRectangle::presetRotationOrderSize(int size) {
    //Allows for space to be allocated for all the rotations before adding them
    if (size <= 0 || size < this->rotationOrderSize) {
        if (DEBUG_MSG_ON || DEBUG_WARNING_MSG_ON) {
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
        //copy all the existing quaternions into the new array and delete them from old array
        for (int i = 0; i < rotationOrderSize; ++i) {
            tempForNewRotOrderArray[i] = rotationOrder[i];
            //Yikes! No! Don't delete the pointers because they are just being transfered to new array:
            //if (rotationOrder[i] != nullptr) {
                //delete rotationOrder[i];
                //rotationOrder[i] = nullptr;
            //}
            delete [] rotationOrder; //Still delete the old array though
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

void CollisionRectangle::addToRotationOrder(const Quaternion & rQuat) { //'rQuat' for rotation quaternion
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
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::Rotation about axis (" << rQuat.getRotationAxis_x() << ", " << rQuat.getRotationAxis_y() << ", " << rQuat.getRotationAxis_z() << ") added to slot " << indexOfFurthestRotationInRotOrder << " in rotation order." << std::endl;
        }
        
        //  INVALID LOGIC I USED TO USE:
        //Iterate backwards through the rotationOrder array until the first non-null rotation quaternion is encountered
//        int rotOrderArrayIter = rotationOrderSize - 1;
//        while (rotationOrder[rotOrderArrayIter] != nullptr && rotOrderArrayIter >= 0) {
//            --rotOrderArrayIter;
//        }
//        if (rotOrderArrayIter >= 0) {
//            rotationOrder[rotOrderArrayIter + 1] = new Quaternion (rQuat);
//            ++numberOfRotations;
//        }
//        else { //Else the rotationOrder array does not contain any actual rotation Quaternions
//            rotationOrder[0] = new Quaternion(rQuat);
//            ++numberOfRotations;
//        }
    }
}
void CollisionRectangle::changeRotationAt(int index, float theta) {
    if (index >= rotationOrderSize || index < 0) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
            std::cout << "Bad index: " << index << std::endl;
        }
        //return;
    }
    else if (rotationOrder[index] == nullptr) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! changeRotationAt(Index = " << index;
            std::cout << ") is attempting to modify a null quaternion.\nCheck your Code!\n";
        }
        //return;
    }
    else {
        rotationOrder[index]->changeTheta(theta);
    }
    doRotationsAndRecalculate();
}
void CollisionRectangle::changeRotitationAxisAt(int index, const Quaternion & rQuat) {
    if (index >= rotationOrderSize || index < 0) {
        if (DEBUG_MSG_ON) {
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
void CollisionRectangle::changeRotitationAxisAt(int index, const aiVector3D & axis, float theta) {
    if (index >= rotationOrderSize || index < 0) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
            std::cout << "Bad index: " << index << std::endl;
        }
        return;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (DEBUG_MSG_ON) {
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
void CollisionRectangle::changeRotitationAxisAt(int index, float x, float y, float z, float theta) {
    if (index >= rotationOrderSize || index < 0) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO CHANGE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
        }
        return;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (DEBUG_MSG_ON) {
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

float CollisionRectangle::getRotationThetaAt(int index) {
    if (index < 0 || index >= rotationOrderSize) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! YOU ARE TRYING TO RETRIEVE THE ROTATION OF A QUATERNION\n";
            std::cout << "IN THE ROTATIONORDER ARRAY AT AN INDEX BEYOND THE SIZE OF THE ARRAY!\n";
            std::cout << "CHECK YOUR COLLISIONRECTANGLE-FUNCTION-CALLING CODE!" << std::endl;
        }
        return 0.0f;
    }
    else {
        if (rotationOrder[index] == nullptr) {
            if (DEBUG_MSG_ON) {
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

void CollisionRectangle::clearRotationOrder() {
    for (int i = 0; i < this->rotationOrderSize; ++i) {
        if (rotationOrder[i] != nullptr) {
            delete rotationOrder[i];
            rotationOrder[i] = nullptr;
        }
    }
    numberOfRotations = 0;
    doRotationsAndRecalculate();
}
void CollisionRectangle::removeRotationAtIndex(int index) {
    if (index < 0 || index > rotationOrderSize) { //If index given is bogus...
        if (DEBUG_MSG_ON) { //Print a debug message is debug messages turned on
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
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::WARNING! RemoveRotationAtIndex(" << index << ") was called\n";
            std::cout << "on this collisionRectangle, but this collisionRectangle does not have\n";
            std::cout << "a rotation quaternion located at that index. Check to make sure removing\n";
            std::cout <<"from the intended index!" << std::endl;
        }
    }
    doRotationsAndRecalculate();
}

int CollisionRectangle::getNumberOfRotations() const {
    return this->numberOfRotations;
}

//this function seems unsafe, so I am not going to include it
//void CollisionRectangle::getRotationOrder(Quaternion *) const {
    
//}

//------------------------------------------------------------------------
//              Collision Detection Functions
//------------------------------------------------------------------------

bool CollisionRectangle::isWithin(float x, float y) {
    if (hasNoArea()) {return false;}
    aiVector2D temp(x,y); //Construct a temp 2D vector from the parameters
    return (isWithin(temp)); //Call alternate version of this function
}
bool CollisionRectangle::isWithin(const aiVector2D & point) {
    if (hasNoArea()) {return false;}
    float greaterX, greaterY, lesserX, lesserY;
    //Need to figure out the bounds of the rectangle from the two corners
    if (corner1.x > corner2.x) {
        greaterX = corner1.x;
        lesserX = corner2.x;
    }
    else {
        greaterX = corner2.x;
        lesserX = corner1.x;
    }
    if (corner1.y > corner2.y) {
        greaterY = corner1.y;
        lesserY = corner2.y;
    }
    else {
        greaterY = corner2.y;
        lesserY = corner1.y;
    }
    //Now just check to see if the point is inside (or on the edge) of the rectangle
    if ( (point.x >= lesserX && point.x <= greaterX) && (point.y >= lesserY && point.y <= greaterY)) {
        return true;
    }
    else {
        return false;
    }
}

bool CollisionRectangle::isOverlapping(const CollisionRectangle& otherRect) {
    //If either rectangle has 0 area, the only way they will be overlapping is if
    if (this->hasNoArea() || otherRect.hasNoArea()) {return ((*this) == otherRect);}
    float thisGreaterX, thisGreaterY, thisLesserX, thisLesserY;
    float otherGreaterX, otherGreaterY, otherLesserX, otherLesserY;
    
    //Do a quick check to see if the rectangles are close enough to warrent a more detailed check.
    float maxSideLength = (x > y ? x : y);
    maxSideLength = (maxSideLength > z ? maxSideLength : z);
    maxSideLength *= scale;
    //So maxSideLength is now the scaled size of the longest side on the rectangle
    
    //If the midpoints between the two rectangles is greater than the largest sidelength, then it
    //isn't possible for them to be overlapping
    if (maxSideLength < this->getDistanceBetweenMidpoints_Scalar(otherRect)) {
        return false; //Stop checking and return false
    }
    
    //Compute the limits of this rectangle
    if (corner1.x > corner2.x) {
        thisGreaterX = corner1.x;
        thisLesserX = corner2.x;
    }
    else {
        thisGreaterX = corner2.x;
        thisLesserX = corner1.x;
    }
    if (corner1.y > corner2.y) {
        thisGreaterY = corner1.y;
        thisLesserY = corner2.y;
    }
    else {
        thisGreaterY = corner2.y;
        thisLesserY = corner1.y;
    }
    //Compute the limits of the other rectangle
    if (otherRect.corner1.x > otherRect.corner2.x) {
        otherGreaterX = otherRect.corner1.x;
        otherLesserX = otherRect.corner2.x;
    }
    else {
        otherGreaterX = otherRect.corner2.x;
        otherLesserX = otherRect.corner1.x;
    }
    if (otherRect.corner1.y > otherRect.corner2.y) {
        otherGreaterY = otherRect.corner1.y;
        otherLesserY = otherRect.corner2.y;
    }
    else {
        otherGreaterY = otherRect.corner2.y;
        otherLesserY = otherRect.corner1.y;
    }
    
    //BEGIN ACTUALLY CHECKING HERE:
    //Check the four corners of this rectangle to see if any are inside the other rectangle
    //Check this corner GreaterX GreaterY
    if ((thisGreaterX <= otherGreaterX && thisGreaterX >= otherLesserX) && (thisGreaterY <= otherGreaterY && thisGreaterY >= otherLesserY)) {
        return true; //return true if the point is inside the other rectangle
    }
    //Check this corner GreaterX Lesser Y
    if ((thisGreaterX <= otherGreaterX && thisGreaterX >= otherLesserX) && (thisLesserY <= otherGreaterY && thisLesserY >= otherLesserY)) {
        return true;
    }
    //Check this corner LesserX Greater Y
    if ((thisLesserX <= otherGreaterX && thisLesserX >= otherLesserX) && (thisGreaterY <= otherGreaterY && thisGreaterY >= otherLesserY)) {
        return true; //return true if the point is inside the other rectangle
    }
    //Check this corner LesserX LesserY
    if ((thisLesserX <= otherGreaterX && thisLesserX >= otherLesserX) && (thisLesserY <= otherGreaterY && thisLesserY >= otherLesserY)) {
        return true;
    }
    
    
    //Check the other rectangle to see if it is inside of this rectangle
    //Check other corner GreaterX GreaterY
    if ((otherGreaterX <= thisGreaterX && otherGreaterX >= thisLesserX) && (otherGreaterY <= thisGreaterY && otherGreaterY >= thisLesserY)) {
        return true; //return true if the point is inside the other rectangle
    }
    //Check other corner GreaterX Lesser Y
    if ((otherGreaterX <= thisGreaterX && otherGreaterX >= thisLesserX) && (otherLesserY <= thisGreaterY && otherLesserY >= thisLesserY)) {
        return true;
    }
    //Check this corner LesserX Greater Y
    if ((otherLesserX <= thisGreaterX && otherLesserX >= thisLesserX) && (otherGreaterY <= thisGreaterY && otherGreaterY >= thisLesserY)) {
        return true; //return true if the point is inside the other rectangle
    }
    //Check this corner LesserX LesserY
    if ((otherLesserX <= thisGreaterX && otherLesserX >= thisLesserX) && (otherLesserY <= thisGreaterY && otherLesserY >= thisLesserY)) {
        return true;
    }
    return false; //Return false if all checks fail to show one rectangle inside the other
}

float CollisionRectangle::getDistanceBetweenMidpoints_Scalar(const CollisionRectangle &otherRect) const{
    float xDistance, yDistance;
    //xDistance = abs(this->midpoint.x - otherRect.midpoint.x); //Do I need ABS if I am squaring values anyways?
    //yDistance = abs(this->midpoint.y - otherRect.midpoint.y);
    //test:
    xDistance = this->midpoint.x - otherRect.midpoint.x;
    yDistance = this->midpoint.y - otherRect.midpoint.y;
    return sqrt((xDistance * xDistance) + (yDistance * yDistance));
}

float CollisionRectangle::getClosestDistanceTo_Scalar(const CollisionRectangle&) const {
    //This one could take a while to implement, need to construct 8 lines using equation of line, then test x points along each
    // line to get the shortest distane from that point to each of the four lines in the other rectangle
    if (DEBUG_MSG_ON) {
        std::cout << "\nDEBUG::OOPS! THIS FUNCTION HASN'T BEEN IMPLEMENTED YET!\n";
    }
    return 0.0f;
}



void CollisionRectangle::moveApartAlongAxisBetweenClosestDetectedPoints(CollisionRectangle &otherRect) {
    //todo (use the closestDistanceTo function
    if (DEBUG_MSG_ON) {
        std::cout << "\nDEBUG::OOPS! THIS FUNCTION HASN'T BEEN IMPLEMENTED YET!\n";
    }
}

//My intention was for this function to be called if both objects were moving when they collide
void CollisionRectangle::moveApartAlongAxisBetweenMidpoints(CollisionRectangle &otherRect) {
    if (!(this->hasModelData) || !(otherRect.hasModelData)) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpoints ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    //float stepsize = STEP_SIZE; //Just use the constant directly
    aiVector2D displacement(this->midpoint.x - otherRect.midpoint.x, this->midpoint.y - otherRect.midpoint.y);
    if (displacement.Length() == 0.0f) {return;}
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        this->midpoint += displacement * STEP_SIZE;
        //Move the other rectangle away also by step amount
        otherRect.midpoint -= displacement * STEP_SIZE;
        this->calculateSelfAfterTranslations();
        otherRect.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherRect));
}

//My intention was for this to be used if the otherRect was stationary when the collision happens
void CollisionRectangle::moveApartAlongAxisBetweenMidpointsThisOnly(CollisionRectangle & otherRect) {
    if (!(this->hasModelData) || !(otherRect.hasModelData)) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpointsThisOnly ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    aiVector2D displacement(this->midpoint.x - otherRect.midpoint.x, this->midpoint.y - otherRect.midpoint.y);
    if (displacement.Length() == 0.0f) {return;}
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        this->midpoint += displacement * STEP_SIZE;
        this->calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherRect));
}

//My intention for this function was for the other rectangle to move if this rectangle was stationary when collision occured
void CollisionRectangle::moveApartAlongAxisBetweenMidpointsOtherOnly(CollisionRectangle & otherRect) {
    if (!(this->hasModelData) || !(otherRect.hasModelData)) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "\nDEBUG::WARNING! MoveApartAlongAxisBetweenMidpointsOtherOnly ";
            std::cout << " called but one of\nthe two CollisionsRectangles never had ";
            std::cout << "itself set from model data!";
            std::cout << std::endl;
        }
        return;
    }
    
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    aiVector2D displacement(this->midpoint.x - otherRect.midpoint.x, this->midpoint.y - otherRect.midpoint.y);
    if (displacement.Length() == 0.0f) {return;}
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        otherRect.midpoint -= displacement * STEP_SIZE;
        otherRect.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherRect));
}


//------------------------------------------------------------------------
//              Getters for drawing collision rectangle
//------------------------------------------------------------------------
/*!
 @remark  This function returns the x,y coordinates for the 4 corners of the collision rectangle
 @remark  This function does not include zValues in the array
 */
void CollisionRectangle::getRectCornersPoints(float * bufferOfEightFloats) const {
    if (!hasModelData) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! Attempting to get data from a ";
            std::cout << "CollisionRectangle that never was set to data!";
            std::cout << std::endl;
        }
    }
    else if (0.0f == this->scale) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! This CollisionRectangle has a scale ";
            std::cout << "set to 0.0f!\nThis could cause a divide by 0.0f to ";
            std::cout << "occur! Please set scale using function setScale().";
            std::cout << std::endl;
        }
    }
    //Corner 1                                             v2
    bufferOfEightFloats[0] = corner1.x; //      +----------+
    bufferOfEightFloats[1] = corner1.y; //      |          |
    //Corner 2                          //      |          |
    bufferOfEightFloats[2] = corner1.x; //      |          |
    bufferOfEightFloats[3] = corner2.y; //      |          |
    //Corner 3                          //      |          |
    bufferOfEightFloats[4] = corner2.x; //      |          |
    bufferOfEightFloats[5] = corner2.y; //      +----------+
    //Corner 4                          //    v1
    bufferOfEightFloats[6] = corner2.x; //              Where the two corners are on the actual rectangle is not fixed
    bufferOfEightFloats[7] = corner1.y; //
}

/*!
 @remark  This function is intended to be used with gl_DrawArrays(GL_TRIANGLES, etc...)
 @remark  This function includes Z values in the array
 */
void CollisionRectangle::getRectCornersTriangles(float * bufferOfEighteenFloats) const {
    if (!hasModelData) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! Attempting to get data from a ";
            std::cout << "CollisionRectangle that never was set to data!";
            std::cout << std::endl;
        }
    }
    else if (0.0f == this->scale) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! This CollisionRectangle has a scale ";
            std::cout << "set to 0.0f!\nThis could cause a divide by 0.0f to ";
            std::cout << "occur! Please set scale using function setScale().";
            std::cout << std::endl;
        }
    }
    float zVal = 0.0f; //Choose a zValue to use
    //Triangle 1
    bufferOfEighteenFloats[0] = corner1.x;
    bufferOfEighteenFloats[1] = corner1.y;
    bufferOfEighteenFloats[2] = zVal;
    bufferOfEighteenFloats[3] = corner1.x;
    bufferOfEighteenFloats[4] = corner2.y;
    bufferOfEighteenFloats[5] = zVal;
    bufferOfEighteenFloats[6] = corner2.x;
    bufferOfEighteenFloats[7] = corner2.y;
    bufferOfEighteenFloats[8] = zVal;
    
    //Triangle 2
    bufferOfEighteenFloats[9] = corner2.x;
    bufferOfEighteenFloats[10] = corner2.y;
    bufferOfEighteenFloats[11] = zVal;
    bufferOfEighteenFloats[12] = corner2.x;
    bufferOfEighteenFloats[13] = corner1.y;
    bufferOfEighteenFloats[14] = zVal;
    bufferOfEighteenFloats[15] = corner1.x;
    bufferOfEighteenFloats[16] = corner1.y;
    bufferOfEighteenFloats[17] = zVal;
}

/*!
 @remark  This code is intended to be used to easily do a Draw_Lines command from GL_DRAWARRAY
 */
void CollisionRectangle::getRectCornersLines(float * bufferOfTwentyFourFloats) const {
    if (!hasModelData) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! Attempting to get data from a ";
            std::cout << "CollisionRectangle that never was set to data!";
            std::cout << std::endl;
        }
    }
    else if (0.0f == this->scale) {
        if (DEBUG_WARNING_MSG_ON) {
            std::cout << "DEBUG::WARNING! This CollisionRectangle has a scale ";
            std::cout << "set to 0.0f!\nThis could cause a divide by 0.0f to ";
            std::cout << "occur! Please set scale using function setScale().";
            std::cout << std::endl;
        }
    }
    float zVal = 0.0f;
    //Side 1
    bufferOfTwentyFourFloats[0] = corner1.x;
    bufferOfTwentyFourFloats[1] = corner1.y;
    bufferOfTwentyFourFloats[2] = zVal;
    bufferOfTwentyFourFloats[3] = corner1.x;
    bufferOfTwentyFourFloats[4] = corner2.y;
    bufferOfTwentyFourFloats[5] = zVal;
    //Side 2
    bufferOfTwentyFourFloats[6] = corner1.x;
    bufferOfTwentyFourFloats[7] = corner2.y;
    bufferOfTwentyFourFloats[8] = zVal;
    bufferOfTwentyFourFloats[9] = corner2.x;
    bufferOfTwentyFourFloats[10] = corner2.y;
    bufferOfTwentyFourFloats[11] = zVal;
    //Side 3
    bufferOfTwentyFourFloats[12] = corner2.x;
    bufferOfTwentyFourFloats[13] = corner2.y;
    bufferOfTwentyFourFloats[14] = zVal;
    bufferOfTwentyFourFloats[15] = corner2.x;
    bufferOfTwentyFourFloats[16] = corner1.y;
    bufferOfTwentyFourFloats[17] = zVal;
    //Side 4
    bufferOfTwentyFourFloats[18] = corner2.x;
    bufferOfTwentyFourFloats[19] = corner1.y;
    bufferOfTwentyFourFloats[20] = zVal;
    bufferOfTwentyFourFloats[21] = corner1.x;
    bufferOfTwentyFourFloats[22] = corner1.y;
    bufferOfTwentyFourFloats[23] = zVal;
}

void CollisionRectangle::get3DRectCornersLines(float * bufferOfTwentyfourFloats) const {
    for (int i = 0; i < 24; i += 3) {
        bufferOfTwentyfourFloats[i] = scale * corners[i%3].x;
        bufferOfTwentyfourFloats[i+1] = scale * corners[i%3].y;
        bufferOfTwentyfourFloats[i+2] = scale * corners[i%3].z;
    }
}

//------------------------------------------------------------------------
//              Private Functions
//------------------------------------------------------------------------

//void CollisionRectangle::calculateSelfBeforeTransformations() { //Calculates self before rotations from original XYZ and midpoint
//    //Assume that maxFromModelXYZ has already been set
//    x = maxFromModelXYZ.x;
//    y = maxFromModelXYZ.y;
//    z = maxFromModelXYZ.z;
//
//    float midX = midpoint.x;
//    float midY = midpoint.y;
//    corner1 = aiVector2D(midX + scale * x, midY + scale * y);
//    corner2 = aiVector2D(midX - scale * x, midY - scale * y);
//}


 //This performs all the rotations in the rotationOrder and then calls computeAfterTranslations()
void CollisionRectangle::doRotationsAndRecalculate() {
    //New (hopefully) not-buggy doRotationsAndRecalculate method:
    //Set the major axes (these are the largest values along each axis for the model
    xAxisMajor = aiVector3D(originalMajorsFromModel.x, 0.0f, 0.0f);
    yAxisMajor = aiVector3D(0.0f, originalMajorsFromModel.y, 0.0f);
    zAxisMajor = aiVector3D(0.0f, 0.0f, originalMajorsFromModel.z);
    //Set the minor axes (these are the largest values of opposite sign from the majors of the model)
    xAxisMinor = aiVector3D(originalMinorsFromModel.x, 0.0f, 0.0f);
    yAxisMinor = aiVector3D(0.0f, originalMinorsFromModel.y, 0.0f);
    zAxisMinor = aiVector3D(0.0f, 0.0f, originalMinorsFromModel.z);

    if (DEBUG_MSG_ON) {
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
         
         if (DEBUG_MSG_ON) {
             std::cout << "Performed rotation #" << i << " on collision box. ";
             std::cout << "Rotated by: " << rotationOrder[i]->getTheta() << "\n";
             aiVector3D rotAxis = rotationOrder[i]->getRotationAxis();
             std::cout << "Rotation axis of rotation " << i << " is: ";
             std::cout << rotAxis.x << ", " << rotAxis.y << ", " << rotAxis.z;
             std::cout << std::endl;
         }
         }
     }
    
    
    //OLDE BUGGY METHOD (This didn't work because I was just rotating a single vector, which represents a cube, not a rectangle)
//    aiVector3D vecToRotate = maxFromModelXYZ;
//
//    if (DEBUG_MSG_ON) {
//        std::cout << "\nDEBUG::Max From Model XYZ is: " << maxFromModelXYZ.x << ",  " << maxFromModelXYZ.y << ",  " << maxFromModelXYZ.z << std::endl;
//    }
//
//    if (numberOfRotations >= 1) {
//       // int rotationCounter = 0;
//        //What I was doing here turned out not to work... so gonna rewrite it
////        //This loop's declaration might look weird, but I worte it this way because there could be null slots anywhere in the
////        //rotationOrderArray. Thus need to go through entire array or until all the rotations in the array have been performed
////        for (int i = 0; ((i < rotationOrderSize) || (rotationCounter > numberOfRotations)) /* &&
////            !((i < rotationOrderSize) && (rotationCounter > numberOfRotations))*/ ; ++i)  {
////            //Since class allows deleteing rotations at any index, need to make sure a valid rotation quaternion actually exists
////            if (rotationOrder[i] != nullptr) {
////                ++rotationCounter;
////                vecToRotate = rotationOrder[i]->computeRotation(vecToRotate);
////            }
////        }
//
//        //Let's see if this fixes anything
//        //rotationOrder[3]->changeTheta(rotationOrder[2]->getTheta());
//
//        //here is the simpler (and possibly slightly less efficient) version
//
//        for (int i = 0; i < rotationOrderSize; ++i) {
//            if (rotationOrder[i] != nullptr /* && i != 2*/) {//The && i != 2 is for debug (but it doesnt seem to matter anyways)
//                vecToRotate = rotationOrder[i]->computeRotation(vecToRotate);
//                if (DEBUG_MSG_ON) {
//                    std::cout << "Performed rotation #" << i << " on collision box. Rotated by: " << rotationOrder[i]->getTheta() << "\n";
//                    aiVector3D rotAxis = rotationOrder[i]->getRotationAxis();
//                    std::cout << "Rotation axis of rotation " << i << " is: " << rotAxis.x << ", " << rotAxis.y << ", " << rotAxis.z << std::endl;
//                }
//            }
//        }
//    }
////    this->x = vecToRotate.x;
////    this->y = vecToRotate.y;
////    this->z = vecToRotate.z;
//    //Hmm clearly this won't make a difference...
//    this->x = (aiVector3D(1.0f, 0.0f, 0.0f) * dot(aiVector3D(1.0f, 0.0f, 0.0f), vecToRotate)).x;
//    this->y = (aiVector3D(0.0f, 1.0f, 0.0f) * dot(aiVector3D(0.0f, 1.0f, 0.0f), vecToRotate)).y;
//    this->z = (aiVector3D(0.0f, 0.0f, 1.0f) * dot(aiVector3D(0.0f, 0.0f, 1.0f), vecToRotate)).z;
//
//    //More debug:
//    if (DEBUG_MSG_ON) {
//        std::cout << "Vector after rotations: " << x << " " << y << " " << z << std::endl;
//    }
    
    calculateSelfAfterTranslations(); //Set the corners after rotating
}

void CollisionRectangle::calculateSelfAfterTranslations() { //Recalculates corner1 and corner2 from midpoint and rotatedXYZ
    
    //new ComputeAfterTranslations code that constructs the 2D collision box from the 6 rotated 3D collision cuboid vectors
    //First I am going to need to construct the 8 cuboid corners from the rotated axis vectors
    aiVector3D cornerArray[8];
    cornerArray[0] = xAxisMajor + yAxisMajor + zAxisMajor;
    cornerArray[1] = xAxisMajor + yAxisMajor + zAxisMinor;
    
    cornerArray[2] = xAxisMajor + yAxisMinor + zAxisMajor;
    cornerArray[3] = xAxisMajor + yAxisMinor + zAxisMinor;
    
    cornerArray[4] = xAxisMinor + yAxisMajor + zAxisMajor;
    cornerArray[5] = xAxisMinor + yAxisMajor + zAxisMinor;
    
    cornerArray[6] = xAxisMinor + yAxisMinor + zAxisMajor;
    cornerArray[7] = xAxisMinor + yAxisMinor + zAxisMinor;
    
    //-----------------THIS NEXT PART COULD BE BUGGY IF MY MATH IS WRONG!--------------  (so close!)
    //Need to get the four points that represent the extremes along x and y axes
    int largestXIndxPos, largestXIndxNeg, largestYIndxPos, largestYIndxNeg;
    largestXIndxPos = largestXIndxNeg = largestYIndxPos = largestYIndxNeg = 0;
    
    //Now loop through the array to find which vectors in the cornerArray contain these maximums
    float maxXPos, maxXNeg, maxYPos, maxYNeg;
    maxXPos = maxXNeg = maxYPos = maxYNeg = 0.0f;
    for (int i = 0; i < 8; ++i) {
        //get largest x
        if (cornerArray[i].x > maxXPos) {
            maxXPos = cornerArray[i].x;
            largestXIndxPos = i;
        }
        //get most negative x
        if (cornerArray[i].x < maxXNeg) {
            maxXNeg = cornerArray[i].x;
            largestXIndxNeg = i;
        }
        //get largest y
        if (cornerArray[i].y > maxYPos) {
            maxYPos = cornerArray[i].y;
            largestYIndxPos = i;
        }
        //get most negative y
        if (cornerArray[i].y < maxYNeg) {
            maxYNeg = cornerArray[i].y;
            largestYIndxNeg = i;
        }
        this->corners[i] = cornerArray[i];
        std::cout << "\nCorner[" << i << "] = " << corners[i].x << ", " << corners[i].y << ", " << corners[i].z;
    }
    
    //This method here doesn't quite work:
    ////now can set corner1 and corner2 from these values
    //corner1 = aiVector2D(scale*(midpoint.x + maxXPos), scale * (midpoint.y + maxYPos));
    //corner2 = aiVector2D(scale * (midpoint.x + maxYNeg), scale * (midpoint.y + maxYNeg));
    
    
    //So instead, calculate the centriod of the 4 maximum vectors that came out of the loops, and then just shift the
    //midpoint over to be at the centriod, then just using 2 vectors should work (hopefully)
    //See: https://en.wikipedia.org/wiki/Centroid
    
    aiVector2D point1(maxXPos/4.0f, maxYPos/4.0f);
    aiVector2D point2(maxXPos/4.0f, maxYNeg/4.0f);
    aiVector2D point3(maxXNeg/4.0f, maxYPos/4.0f);
    aiVector2D point4(maxXNeg/4.0f, maxYNeg/4.0f);
    
    aiVector2D centriod = point1 + point2 + point3 + point4;
    
    //std::cout << "\nCENTRIOD IS: " << centriod.x << ", " << centriod.y << " and scale is: " << scale;
    
    //here goes:
//    corner1 = aiVector2D(scale*(midpoint.x + centriod.x + maxXPos), scale * (midpoint.y + centriod.y + maxYPos));
//    corner2 = aiVector2D(scale * (midpoint.x + centriod.x + maxXNeg), scale * (midpoint.y + centriod.y + maxYNeg));
    corner1 = aiVector2D(scale*(midpoint.x + maxXPos), scale * (midpoint.y + maxYPos));
    corner2 = aiVector2D(scale * (midpoint.x + maxXNeg), scale * (midpoint.y + maxYNeg));
    
    
    
    //Ye Olde calculateSelfAfterTranslations to go with the olde buggy computeRotations code above
    //    float midX = midpoint.x;
//    float midY = midpoint.y;
//    //corner1 = aiVector2D(midX + scale * x, midY + scale * y);
//    //corner2 = aiVector2D(midX - scale * x, midY - scale * y);
//
//    corner1 = aiVector2D(scale*(midX + x), scale *(midY + y));
//    corner2 = aiVector2D(scale*(midX - x), scale *(midY - y));
}

bool CollisionRectangle::hasNoArea() const {
    //It is assumed that whenever a translation or rotation (or anything) happens,
    //corner1 and corner2 will be recalculated, thus meaning they are always accurate descriptions
    //of an instance's collision
    if (!hasModelData) {return true;}
    //Else do a calculation to see if the box actually has no area for some reason
    return (abs(corner1.x-corner2.x) * abs(corner1.y - corner2.y) == 0.0f);
}

/*!
 This Code is for finding the largest absolute value in a collection of floats.
 @remark This just returns the maximum value in an array of floats using absolue value
 */
float CollisionRectangle::getMaxFromArray(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (abs(data[i]) > max) {
            max = abs(data[i]);
        }
    }
    return max;
}

float CollisionRectangle::getMaxFromArrayPositiveOnly(float * data, int dataSize) {
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
float CollisionRectangle::getMaxFromArrayNegativeOnly(float * data, int dataSize) {
    float negMax = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (data[i] < negMax) {
            negMax = data[i];
        }
    }
    return negMax;
}

