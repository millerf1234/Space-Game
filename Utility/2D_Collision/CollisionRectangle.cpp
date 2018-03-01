//
//  CollisionRectangle.cpp
//
//  Created by Forrest Miller on 2/28/18.
//

#include "CollisionRectangle.h"

static constexpr float STEP_SIZE = 0.01f; //Used for determining how fast to move two collosionRectangles apart

static constexpr bool DEBUG_MSG_ON = true;


//------------------------------------------------------------------------
//              CONSTRUCTORS
//------------------------------------------------------------------------

CollisionRectangle::CollisionRectangle() {
    x = y = z = 0.0f;
    corner1 = corner2 = aiVector2D(0.0f, 0.0f);
    corner1OffsetFromMidpointBeforeRotations = corner2OffsetFromMidpointBeforeRotations = aiVector3D(0.0f, 0.0f, 0.0f);
    midpoint = aiVector2D(0.0f, 0.0f);
    maxFromModelXYZ = aiVector3D(x,y,z);
    rotationOrder = nullptr;
    numberOfRotations = 0;
    rotationOrderSize = 0;
    
    scale = 1.0f;
    //calculateSelfBeforeTransformations(); //Don't need to calculate self if everything is 0?
    
}

/*!
 Constructor that initializes the collisionRectangle straight from the model data. If model has no z component, just
 create an array with x and y coordinates as desired and 0.0f set at every third value. Note that after construction the
 midpoint of the CollisionRectangle still needs to be set to match the models collisionRectangle, and 
 @remark  It is assumed in this constructor that Data is model data in the form x0,y0,z0,x1,y1,z1,x2,y2,z2,...
 */
CollisionRectangle::CollisionRectangle(float * data, int dataPoints) {
    if (dataPoints % 3 != 0) {
        if (DEBUG_MSG_ON) {
            std::cout << "\nDEBUG::OOPS! Attempting to construct a collision rectangle from\ndata that is not a multiple of 3!!";
            std::cout << std::endl;
        } //Throw an exception? Maybe later once I have more classes written that can share exceptions to be thrown
        //(*this) = CollisionRectangle(); //Just call the parameterless constructor
        x = y = z = 0.0f;
        corner1 = corner2 = aiVector2D(0.0f, 0.0f);
        corner1OffsetFromMidpointBeforeRotations = corner2OffsetFromMidpointBeforeRotations = aiVector3D(0.0f, 0.0f, 0.0f);
        midpoint = aiVector2D(0.0f, 0.0f);
        maxFromModelXYZ = aiVector3D(x,y,z);
        rotationOrder = nullptr;
        numberOfRotations = 0;
        rotationOrderSize = 0;
        scale = 1.0f;
        return;
    }
    //These next 3 lines necessary? They just initialize values that are about to be set anyways
    //x = y = z = 0.0f;
    //corner1 = corner2 = aiVector2D(0.0f, 0.0f);
    //corner1OffsetFromMidpointBeforeRotations = corner2OffsetFromMidpointBeforeRotations = aiVector3D(0.0f, 0.0f, 0.0f);
    
    
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
    //potentiall faster method:
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
    
    //Get the max values along each axis of from the model data
    x = getMaxFromArray(xDataPoints, dataPoints / 2); //Get max x value
    y = getMaxFromArray(yDataPoints, dataPoints / 2); //Get max y value
    z = getMaxFromArray(zDataPoints, dataPoints / 2); //Get max z value
    
    this->maxFromModelXYZ = aiVector3D(x,y,z); //Set this vector from the calculated maximums
    
    if (DEBUG_MSG_ON) {
        std::cout << "\nDEBUG::CollisionRectangle Size Values Determined!\nxMax: " << x;
        std::cout << "\nyMax: " << y << "\nzMax: " << z << std::endl;
    }
    calculateSelfBeforeTransformations();
    //Not sure if I need to do this second
    doRotationsAndRecalculate();
}

CollisionRectangle::CollisionRectangle(aiVector3D *, int numberOfVectors){
    
    //Not Yet implemented. Shouldn't be very difficult to implement
    
    
    calculateSelfBeforeTransformations();
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
//              Rotation Functions
//------------------------------------------------------------------------
void CollisionRectangle::presetRotationOrderSize(int size) {
    //Allows for space to be allocated for all the rotations before adding them
    if (size <= 0 || size < this->rotationOrderSize) {
        if (DEBUG_MSG_ON) {
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
        rotationOrder = new Quaternion* [size];
        for (int i = 0; i < size; ++i) {
            rotationOrder[i] = nullptr; //Set each entry to nullptr
        }
        this->rotationOrderSize = size;
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
        //numberOfRotations shoudl remain unchanged
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
        //Iterate backwards through the rotationOrder array until the first non-null rotation quaternion is encountered
        int rotOrderArrayIter = rotationOrderSize - 1;
        while (rotationOrder[rotOrderArrayIter] != nullptr && rotOrderArrayIter >= 0) {
            --rotOrderArrayIter;
        }
        if (rotOrderArrayIter >= 0) {
            rotationOrder[rotOrderArrayIter + 1] = new Quaternion (rQuat);
            ++numberOfRotations;
        }
        else { //Else the rotationOrder array does not contain any actual rotation Quaternions
            rotationOrder[0] = new Quaternion(rQuat);
            ++numberOfRotations;
        }
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
}

void CollisionRectangle::clearRotationOrder() {
    for (int i = 0; i < this->rotationOrderSize; ++i) {
        if (rotationOrder[i] != nullptr) {
            delete rotationOrder[i];
            rotationOrder[i] = nullptr;
        }
    }
    numberOfRotations = 0;
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
    if (maxSideLength < this->getDistanceBetweenMidpoints(otherRect)) {
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


float CollisionRectangle::getDistanceBetweenMidpoints(const CollisionRectangle &otherRect) const{
    float xDistance, yDistance;
    //xDistance = abs(this->midpoint.x - otherRect.midpoint.x); //Do I need ABS if I am squaring values anyways?
    //yDistance = abs(this->midpoint.y - otherRect.midpoint.y);
    //test:
    xDistance = this->midpoint.x - otherRect.midpoint.x;
    yDistance = this->midpoint.y - otherRect.midpoint.y;
    return sqrt((xDistance * xDistance) + (yDistance * yDistance));
}

float CollisionRectangle::closestDistanceTo(const CollisionRectangle&) const {
    //This one could take a while to implement, need to construct 8 lines using equation of line, then test x points along each
    // line to get the shortest distane from that point to each of the four lines in the other rectangle
    if (DEBUG_MSG_ON) {
        std::cout << "\nDEBUG::OOPS! THIS FUNCTION HASN'T BEEN IMPLEMENTED YET!\n";
    }
    return 0.0f;
}

void CollisionRectangle::moveApartAlongAxisBetweenClosestCorners(CollisionRectangle &otherRect) {
    //todo (use the closestDistanceTo function
    if (DEBUG_MSG_ON) {
        std::cout << "\nDEBUG::OOPS! THIS FUNCTION HASN'T BEEN IMPLEMENTED YET!\n";
    }
}

void CollisionRectangle::moveApartAlongAxisBetweenMidpoints(CollisionRectangle &otherRect) {
    //The way I am going to write this is going to be terribly inefficient, kinda a brute force attemp
    //float stepsize = STEP_SIZE; //Just use the constant directly
    aiVector2D displacement(this->midpoint.x - otherRect.midpoint.x, this->midpoint.y - otherRect.midpoint.y);
    if (displacement.Length() == 0.0f) {return;}
    //Start moving the two rectangles apart until they are no longer overlapping
    do {
        //Move this rectangle away from the otherRect midpoint by step amount
        this->midpoint += displacement * STEP_SIZE;
        otherRect.midpoint -= displacement * STEP_SIZE;
        this->calculateSelfAfterTranslations();
        otherRect.calculateSelfAfterTranslations();
    } while (this->isOverlapping(otherRect));
}

void CollisionRectangle::moveApartAlongAxisBetweenMidpointsThisOnly(CollisionRectangle & otherRect) {
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

void CollisionRectangle::moveApartAlongAxisBetweenMidpointsOtherOnly(CollisionRectangle & otherRect) {
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
 */
void CollisionRectangle::getRectCornersPoints(float * bufferOfEightFloats) const {
    //ADD Z VALUES
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
    bufferOfEightFloats[6] = corner2.x; //                    Where the two corners are on the actual rectangle is not fixed
    bufferOfEightFloats[7] = corner1.y; //
}

/*!
 @remark  This function is intended to be used with gl_DrawArrays(GL_TRIANGLES, etc...)
 */
void CollisionRectangle::getRectCornersTriangles(float * bufferOfTwelveFloats) const {
    
    //ADD Z VALUEs
    //Triangle 1
    bufferOfTwelveFloats[0] = corner1.x;
    bufferOfTwelveFloats[1] = corner1.y;
    bufferOfTwelveFloats[2] = corner1.x;
    bufferOfTwelveFloats[3] = corner2.y;
    bufferOfTwelveFloats[4] = corner2.x;
    bufferOfTwelveFloats[5] = corner2.y;
    
    //Triangle 2
    bufferOfTwelveFloats[6] = corner2.x;
    bufferOfTwelveFloats[7] = corner2.y;
    bufferOfTwelveFloats[8] = corner2.x;
    bufferOfTwelveFloats[9] = corner1.y;
    bufferOfTwelveFloats[10] = corner1.x;
    bufferOfTwelveFloats[11] = corner1.y;
}

/*!
 @remark  This code is intended to be used to easily do a Draw_Lines command from GL_DRAWARRAY
 */
void CollisionRectangle::getRectCornersLines(float * bufferOfTwentyFourFloats) const {
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

//------------------------------------------------------------------------
//              Private Functions
//------------------------------------------------------------------------

void CollisionRectangle::calculateSelfBeforeTransformations() { //Calculates self before rotations from original XYZ and midpoint
    //Assume that maxFromModelXYZ has already been set
    x = maxFromModelXYZ.x;
    y = maxFromModelXYZ.y;
    z = maxFromModelXYZ.z;
    //
    corner1OffsetFromMidpointBeforeRotations = aiVector3D(x, y, z); //
    corner2OffsetFromMidpointBeforeRotations = aiVector3D(-x, -y, -z);

    float midX = midpoint.x;
    float midY = midpoint.y;
    corner1 = aiVector2D(midX + scale * x, midY + scale * y);
    corner2 = aiVector2D(midX - scale * x, midY - scale * y);
}

void CollisionRectangle::doRotationsAndRecalculate() { //This performs all the rotations in the rotationOrder and
    aiVector3D vecToRotate = maxFromModelXYZ;
    if (numberOfRotations >= 1) {
        int rotationCounter = 0;
        //This loop's declaration might look weird, but I worte it this way because there could be null slots anywhere in the
        //rotationOrderArray. Thus need to go through entire array or until all the rotations in the array have been performed
        for (int i = 0; ((i < rotationOrderSize) || (rotationCounter == numberOfRotations)) &&
            !((i < rotationOrderSize) || (rotationCounter == numberOfRotations)) ; ++i) {
            //Since class allows deleteing rotations at any index, need to make sure a valid rotation quaternion actually exists
            if (rotationOrder[i] != nullptr) {
                ++rotationCounter;
                vecToRotate = rotationOrder[i]->computeRotation(vecToRotate);
            }
        }
    }
    this->x = vecToRotate.x;
    this->y = vecToRotate.y;
    this->z = vecToRotate.z;
    calculateSelfAfterTranslations(); //Set the corners after rotating
}

void CollisionRectangle::calculateSelfAfterTranslations() { //Recalculates corner1 and corner2 from midpoint and rotatedXYZ
    float midX = midpoint.x;
    float midY = midpoint.y;
    //corner1 = aiVector2D(midX + scale * x, midY + scale * y);
    //corner2 = aiVector2D(midX - scale * x, midY - scale * y);
    corner1 = aiVector2D(scale*(midX + x), scale *(midY + y));
    corner2 = aiVector2D(scale*(midX - x), scale *( midY - y));
}

bool CollisionRectangle::hasNoArea() const {
    //It is assumed that whenever a translation or rotation (or anything) happens,
    //corner1 and corner2 will be recalculated, thus meaning they are always accurate descriptions
    //of an instance's collision
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
