/// THIS CLASS IS INCOMPLETE. NEEDS TO BE COMPLETED...
//  CollisionCircle.cpp
//
//  Created by Forrest Miller on 3/29/18.
//

#include "CollisionCircle.h"











//------------------------------------------------------------------------
//              CONSTRUCTORS
//------------------------------------------------------------------------

////For getting the vertices of the circle, do:
////Load testNGon into vertices
//aiVector3D** vec = nGonTest->getOffsets();
////Put nGon in beginning of vertices
//aiVector3D orgin = nGonTest->getOrgin();
//vertices[0] = orgin.x;
//vertices[1] = orgin.y;
//vertices[2] = orgin.z;
//for (int i = 3; i < nGonTest->getNumSides()*3+3; i+=3) {
//    vertices[i] = (*vec)->x;
//    vertices[i+1] = (*vec)->y;
//    vertices[i+2] = (*vec)->z;
//    ++vec;
//}
////Got to complete the nGon by adding the first offset to the end of verts
//vec = nGonTest->getOffsets();
//int tmp = nGonTest->getNumSides()*3+3;
//vertices[tmp++] = (*vec)->x;
//vertices[tmp++] = (*vec)->y;
//vertices[tmp++] = (*vec)->z;


//------------------------------------------------------------------------
//              Collision Detection/Resolution Functions
//------------------------------------------------------------------------
bool CollisionCircle::isInside(const aiVector2D & point) const {
    //Get the distance from this circles center to the vector
    aiVector2D distance(point.x - centerPoint.x, point.y - centerPoint.y );
    return (distance.Length() < radius);
}

bool CollisionCircle::isInside(float x, float y) const{
    //Get the distance from this circles center to the vector
    aiVector2D distance(x - centerPoint.x, y - centerPoint.y );
    return (distance.Length() < radius);
}

bool CollisionCircle::isInsideOrOnEdge(float x, float y) const {
    //Get the distance from this circles center to the vector
    aiVector2D distance(x - centerPoint.x, y - centerPoint.y );
    return (distance.Length() <= radius);
}

bool CollisionCircle::checkIfLineIntersectsCircle(aiVector2D pointA, aiVector2D pointB) const {
    return checkIfLineIntersectsCircle(pointA.x, pointA.y, pointB.x, pointB.y); //Just call the other version of this function
}

bool CollisionCircle::checkIfLineIntersectsCircle(float x0, float y0, float x1, float y1) const {
    //see:https://math.stackexchange.com/questions/275529/check-if-line-intersects-with-circles-perimeter
    //    in that link above, see Ryu Jin's response (it's a few down from the top)
    //Math is simpler if everything is translated so that the center of the circle becomes the orgin, so do:
    float x0trans = x0 - centerPoint.x;
    float y0trans = y0 - centerPoint.y;
    float x1trans = x1 - centerPoint.x;
    float y1trans = y1 - centerPoint.y;
    //ABC are coefficents of the quadratic equation
    float A = pow(x0trans, 2.0f) + pow(y0trans, 2.0f) - pow(radius, 2.0f);
    float B = 2.0f * (x0trans * (x1trans - x0trans) + y0trans * (y1trans - y0trans));
    float C = pow((x1trans - x0trans), 2.0f) + pow((y1trans - y0trans), 2.0f);
    float discriminant = (B * B) - (4.0f * A * C);
    //if the discriminant is negative, can return false
    if (discriminant < 0.0f) {
        return false;
    }
    //Else, compute the square root of the discriminant
    discriminant = pow(discriminant, 0.5f); //raising to 0.5f power is same as sqrt
    float t0 = (-B + discriminant) / (2.0f * A);
    float t1 = (-B + discriminant) / (2.0f * A);
    if ((0.0f < t0 && t0 < 1.0f ) || (0.0f < t1 && t1 < 1.0f)) {
        return true;
    } //else
    return false;
}

bool CollisionCircle::isOverlapping(const CollisionCircle & other) const {
    //Get the larger circles radius
    float largerRadius = (this->radius > other.radius) ? this->radius : other.radius;
    //Get the distance between the two circles centerpoints
    aiVector2D distance = other.centerPoint - this->centerPoint;
    //See if the distance between the midpoints is less than the larger radius
    if (distance.Length() < largerRadius) {
        return true;
    }
    else {
        return false;
    }
}

bool CollisionCircle::isOverlapping(const CollisionBox & other) const {
    //New plan of attack to figuring this out:
    //  a) check each corner of the box to see if any are inside the circle
    //  b) check to see if the max/min xy values of one shape rule out overlapping with the other
    //  c) check each line of the box to see if any of them intersect the circle
    
    
    //Do part a
    float corners[8];
    other.getRect2DCornerPoints2D(corners); //Get the corners from the CollisionBox
    if (isInsideOrOnEdge(corners[0], corners[1]) ||
        isInsideOrOnEdge(corners[2], corners[3]) ||
        isInsideOrOnEdge(corners[4], corners[5]) ||
        isInsideOrOnEdge(corners[6], corners[7])) {
        return true;
    }
    
    
    //Do part b (NOTE THE WAY I DO THIS IS DEPENDENT ON HOW THE CORNERS OF THE COLOSIONBOX ARE FORMED)
    //Just for sanity testing, print out a warning if it was detected that the collisionBox was formed wrong
    if (corners[0] < corners[4]) {
        std::cout << "\nWARNING! A COLLISION BOX WAS FORMED WITH THE WRONG CORNER ORDERING!\n";
        std::cout << "Corners[0] = " << corners[0] << " while Corners[4] = " << corners[4] << std::endl;
    }
    if (corners[3] < corners[7]) {
        std::cout << "\nWARNING! A COLLISION BOX WAS FORMED WITH THE WRONG CORNER ORDERING!\n";
        std::cout << "Corners[3] = " << corners[3] << " while Corners[7] = " << corners[7] << std::endl;
    }
    //Note that the opposite two cases for sanity testing are covered by the first two cases
    aiVector2D circleMaxX = centerPoint + aiVector2D(radius, 0.0f);
    if (circleMaxX.x < corners[4]) { //Corners[4] will be the box's most negative x coordinate
        return false;
    }
    //Check to see if the most positive y coordinate of the circle is less than the most negative y coordinate of the box
    aiVector2D circleMaxY = centerPoint + aiVector2D(0.0f, radius);
    if (circleMaxY.y < corners[7]) { //Corners[7] will be the box's most negative y coordinate
        return false;
    }
    //Check to see if the most negative x coordinate of the circle is greater than the most positive x coordinate of the box
    aiVector2D circleMinX = centerPoint - aiVector2D(radius, 0.0f);
    if (circleMinX.x > corners[0]) { //Corners[0] will be the box's most positive x coordinate
        return false;
    }
    //Check to see if the most negative y coordinate of the circle is greater than the most positive y coordinate of the box
    aiVector2D circleMinY = centerPoint - aiVector2D(0.0f, radius);
    if (circleMinY.y > corners[3]) { //Corners[3] will be the box's most positive y coordinate
        return false;
    }
    
    //Do part c
    
    
    
    return false;
    
    //Other idea I had for doing this check: try to form a seperating axis between the two shapes
}

void CollisionCircle::moveApartAlongLineBetweenMidpoints(CollisionCircle & other) {
    float stepsize = 0.01f;
    //Get the vector between the two circle's centerpoints
    aiVector2D vecBetweenCenters(other.centerPoint.x - this->centerPoint.x, other.centerPoint.y - this->centerPoint.y);
    do {
        this->centerPoint -= (vecBetweenCenters * stepsize);
        other.centerPoint += (vecBetweenCenters * stepsize);
    } while (this->isOverlapping(other));
    
}
void CollisionCircle::moveApartAlongLineBetweenMidpoints(CollisionBox & other) {
    float stepsize = 0.01f;
    //Get the vector between this circles center point and the box's midpoint
    aiVector2D vecBetweenMidpoints(this->centerPoint.x - other.getMidpoint().x,
                                   this->centerPoint.y - other.getMidpoint().y);
    do {
        this->centerPoint -= (vecBetweenMidpoints * stepsize);
        other.translate(vecBetweenMidpoints * stepsize); //Translate the other box
    } while (this->isOverlapping(other));
}
void CollisionCircle::moveApartAdvanced(CollisionBox & other) {
    //The idea here is to find a side of the collision box that is touching
    //or enters inside the circle. If it is touching, then the vector that
    //the two collision shapes will move apart along is from the circle center
    //to the place where the box touches. If the box intersects the circle
    //in 2 places to form a chord, the vector is from the center of the circle
    //to the midpoint of this chord
    
    //Get the rectangle corner coordinates
    float boxCorners[8];
    other.getRect2DCornerPoints2D(boxCorners); //Have box place it's corners in the array

    //Now check to see if any of the corners on the box are inside the sphere
    bool corner0Inside, corner1Inside, corner2Inside, corner3Inside;
    corner0Inside = corner1Inside = corner2Inside = corner3Inside = false;
    
    short numberOfCornersInside = 0;
    
    if (isInside(boxCorners[0], boxCorners[1])) {
        corner0Inside = true;
        numberOfCornersInside++;
    }
    if (isInside(boxCorners[2], boxCorners[3])) {
        corner1Inside = true;
        numberOfCornersInside++;
    }
    if (isInside(boxCorners[4], boxCorners[5])) {
        corner2Inside = true;
        numberOfCornersInside++;
    }
    if (isInside(boxCorners[6], boxCorners[7])) {
        corner3Inside = true;
        numberOfCornersInside++;
    }
    
//    //Check to see if 3 or more corners are inside the circle
//    if ( (corner0Inside && corner1Inside && corner2Inside) ||
//         (corner0Inside && corner1Inside && corner3Inside) ||
//         (corner0Inside && corner2Inside && corner3Inside) ||
//         (corner1Inside && corner2Inside && corner3Inside) ) {
//        //then just go with the move-apart-along-line-between-midpoints method
//        return moveApartAlongLineBetweenMidpoints(other);
//    }
    //Instead, check the counter
    if (numberOfCornersInside >= 3) {
        return moveApartAlongLineBetweenMidpoints(other);
    }
    
    else if (numberOfCornersInside == 2) {
        
    }
    
    else if (numberOfCornersInside == 1) {
        
    }
    
    else if (numberOfCornersInside == 0) {
        //see:https://math.stackexchange.com/questions/275529/check-if-line-intersects-with-circles-perimeter
        //Use the algorithm described in the above link
        //float x0 = this->centerPoint.x;
        //float y0 = this->centerPoint.y;
        //Check the line from corner 0 to corner 1
    }
}


//------------------------------------------------------------------------
//             Private Helper Functions
//------------------------------------------------------------------------

void recalculateRadius();

void initialize(); //This function to be called by constructors
