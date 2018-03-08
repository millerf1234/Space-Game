//
//  RectangularCuboid.cpp
//
//  Created by Forrest Miller on 2/27/18.
//

#include "RectangularCuboid.h"

static constexpr bool USE_DEBUG_MESSAGES = true;


//Public RectangularCuboid Functionality Implementation:

//Constructors
RectangularCuboid::RectangularCuboid() {
    this->length = 0.0f;
    this->width = 0.0f;
    this->height = 0.0f;
    
    this->earlyZRot = new Quaternion(0.0f, 0.0f, 1.0f);
    this->xRot = new Quaternion(1.0f, 0.0f, 0.0f);
    this->yRot = new Quaternion(0.0f, 1.0f, 0.0f);
    this->zRot = new Quaternion(0.0f, 0.0f, 1.0f);
    this->cPlaneRotation = 0.0f;
    this->midpoint = aiVector3D(0.0f, 0.0f, 0.0f);
    
    cPlane.vec1 = aiVector3D(1.0f, 0.0f, 0.0f);
    cPlane.vec2 = aiVector3D(0.0f, 1.0f, 0.0f);
    
    for (int i = 0; i < NUM_CUBE_VERTICES; ++i) {
        vertices[i] = 0.0f;
        originalVerticies[i] = 0.0f;
    }
}



//Destructors



//Getters
float RectangularCuboid::getEarlyZRotAngle() const {
    if (earlyZRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getEarlyZRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return earlyZRot->getTheta();
}

float RectangularCuboid::getXRotAngle() const {
    if (xRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getXRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return xRot->getTheta();
}

float RectangularCuboid::getYRotAngle() const {
    if (yRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getYRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return yRot->getTheta();
}

float RectangularCuboid::getZRotAngle() const {
    if (zRot == nullptr) {
        if (USE_DEBUG_MESSAGES) {
            std::cout << "\nDEBUG::OOPS! getzRotAngle() was called on a RectangularCuboid but for some\nreason the rotation quaternion for this rotation is null!" << std::endl;
        }
        return -999999.9f;
    }
    //else
    return zRot->getTheta();
}



/*
 static constexpr int NUM_CUBE_VERTICES = 8;
 
 class RectangularCuboid {
 private:
 float length, width, height; //Convention: Length follows x-Axis, Height follows y-Axis, Width follows z-Axis
 
 aiVector3D midpoint;
 
 
 float vertices[NUM_CUBE_VERTICES];
 float originalVerticies[NUM_CUBE_VERTICES];
 
 void rotate();
 
 Quaternion * earlyZRot, * xRot, * yRot, * zRot;
 
 public:
 //Constructors
 RectangularCuboid();
 RectangularCuboid(float * objData, int size); //Construct a cube from model data, model will be contained in cube.
 RectangularCuboid(const aiVector3D &);
 //Destructors
 ~RectangularCuboid();
 
 //Public functions
 bool isInside(float x, float y); //Checks to see if the given x,y coordinates are inside the cube
 bool isInside(const RectangularCuboid &); //Checks to see if two rectangular cuboid's are overlapping
 RectangularCuboid(float * objData, int posIndexOffset, int vertComponents, int numDatapoints);
 
 float getCenterDistances(const RectangularCuboid &); //Returns the distance between the midpoints of 2 cuboids
 float getCenterDistanceTo(const aiVector2D &); //Assumes that aiVector2D lies in the same depth plane
 float getCenterDistanceTo(const aiVector3D &);
 
 void updateRotationAngles(float earlyZ, float x, float y, float z);
 void updateRotationAngles(float earlyZ, const aiVector3D &);
 void updateRotationAngleX(float x);
 void updateRotationAngleY(float y);
 void updateRotationAngleZ(float z);
 void updateRotationAngleEarlyZ(float ez);
 
 
 void initFromModel(float *, int);
 
 float getEarlyZRotAngle() const;
 float getXRotAngle() const;
 float getYRotAngle() const;
 float getZRotAngle() const;
 };
 */
 

//Large function from other collision box class that I am not keeping most of
////
//void AACollisionBox::calculateSelfAfterTranslations() {
//
//    //New new version
//    //SO corners3D should have been set
//    aiVector3D modelTranslatedCorners3D[8];
//    aiVector3D midpoint3D = aiVector3D(midpoint.x, midpoint.y, 0.0f);
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        //corners3D[i] = this->corners3D[i];
//        modelTranslatedCorners3D[i] = (midpoint3D + (this->corners3D[i] * collisionBoxShrinkageFactor))  * scale;
//    }
//
//    float maxX, minX, maxY, minY;
//    maxX = minX = scale * midpoint.x;
//    maxY = minY = scale * midpoint.y;
//    int maxXIndex, minXIndex, maxYIndex, minYIndex;
//    maxXIndex = minXIndex = maxYIndex = minYIndex = NOT_SET;
//
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        if (modelTranslatedCorners3D[i].x >= maxX) {
//            maxX = modelTranslatedCorners3D[i].x;
//            maxXIndex = i;
//        }
//        else if (modelTranslatedCorners3D[i].x <= minX) {
//            minX = modelTranslatedCorners3D[i].x;
//            minXIndex = i;
//        }
//
//        if (modelTranslatedCorners3D[i].y >= maxY) {
//            maxY = modelTranslatedCorners3D[i].y;
//            maxYIndex = i;
//        }
//        else if (modelTranslatedCorners3D[i].y <= minY) {
//            minY = modelTranslatedCorners3D[i].y;
//            minYIndex = i;
//        }
//    }
//    corners2D[0] = aiVector2D(modelTranslatedCorners3D[maxXIndex].x, modelTranslatedCorners3D[maxXIndex].y); //Max x
//    corners2D[1] = aiVector2D(modelTranslatedCorners3D[maxYIndex].x, modelTranslatedCorners3D[maxYIndex].y); //Max Y
//    corners2D[2] = aiVector2D(modelTranslatedCorners3D[minXIndex].x, modelTranslatedCorners3D[minXIndex].y); //Min x
//    corners2D[3] = aiVector2D(modelTranslatedCorners3D[minYIndex].x, modelTranslatedCorners3D[minYIndex].y); //Min Y
//    return;
//
//    /*
//
//     //This function updates the values in the corners2D and corners3D arrays
//
//     aiVector3D unorderedCornersArray[CUBOID_CORNERS];
//     unorderedCornersArray[0] = xAxisMajor + yAxisMajor + zAxisMajor;
//     unorderedCornersArray[1] = xAxisMajor + yAxisMajor + zAxisMinor;
//     unorderedCornersArray[2] = xAxisMajor + yAxisMinor + zAxisMajor;
//     unorderedCornersArray[3] = xAxisMajor + yAxisMinor + zAxisMinor;
//     unorderedCornersArray[4] = xAxisMinor + yAxisMajor + zAxisMajor;
//     unorderedCornersArray[5] = xAxisMinor + yAxisMajor + zAxisMinor;
//     unorderedCornersArray[6] = xAxisMinor + yAxisMinor + zAxisMajor;
//     unorderedCornersArray[7] = xAxisMinor + yAxisMinor + zAxisMinor;
//
//     //new new way of calculating the four corners
//     //Find the 2D centriod of all 8 vectors
//
//     aiVector2D point1(unorderedCornersArray[0].x / 8.0f, unorderedCornersArray[0].y / 8.0f);
//     aiVector2D point2(unorderedCornersArray[1].x / 8.0f, unorderedCornersArray[1].y / 8.0f);
//     aiVector2D point3(unorderedCornersArray[2].x / 8.0f, unorderedCornersArray[2].y / 8.0f);
//     aiVector2D point4(unorderedCornersArray[3].x / 8.0f, unorderedCornersArray[3].y / 8.0f);
//     aiVector2D point5(unorderedCornersArray[4].x / 8.0f, unorderedCornersArray[4].y / 8.0f);
//     aiVector2D point6(unorderedCornersArray[5].x / 8.0f, unorderedCornersArray[5].y / 8.0f);
//     aiVector2D point7(unorderedCornersArray[6].x / 8.0f, unorderedCornersArray[6].y / 8.0f);
//     aiVector2D point8(unorderedCornersArray[7].x / 8.0f, unorderedCornersArray[7].y / 8.0f);
//
//     aiVector2D centriod = point1 + point2 + point3 + point4 + point5 + point6 + point7 + point8;
//
//     //for debug:
//     int cindx1, cindx2, cindx3, cindx4;
//     cindx1 = cindx2 = cindx3 = cindx4 = -1;
//
//     aiVector2D centriodToCorners[CUBOID_CORNERS];
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     centriodToCorners[i] = aiVector2D(unorderedCornersArray[i].x, unorderedCornersArray[i].y) - centriod;
//     }
//
//     for (int i = 0; i < BOX_CORNERS; ++i) {
//     corners2D[i] = aiVector2D(0.0f, 0.0f);
//     }
//
//     //Okay so this works if roll is at -90º, 0º or 90º, but fails all other times
//     //Need to find the four largest vectors in each quadrant
//     //Loop through the corner array again
//     for (int i = 0; i < CUBOID_CORNERS; i+=1) {
//     if (centriodToCorners[i].x > 0.0f) {
//     if (centriodToCorners[i].y > 0.0f) {
//     //Then vector is in quadrant 1
//     //Check to see if it is the longest
//     if (corners2D[0].Length() < centriodToCorners[i].Length()) {
//     corners2D[0].x = unorderedCornersArray[i].x;
//     corners2D[0].y = unorderedCornersArray[i].y;
//     cindx1 = i;
//     //corners2D[0] = centriodToCorners[i];
//     //                    centriodToCorners[i].x = 0.0f;
//     //                    centriodToCorners[i].y = 0.0f;
//     }
//     }
//     else {
//     //Then Vector is in quadrant 4
//     if (corners2D[1].Length() < centriodToCorners[i].Length()) {
//     corners2D[1].x = unorderedCornersArray[i].x;
//     corners2D[1].y = unorderedCornersArray[i].y;
//     //                    corners2D[1] = centriodToCorners[i];
//     //                    centriodToCorners[i].x = 0.0f;
//     //                    centriodToCorners[i].y = 0.0f;
//     cindx2 = i;
//     }
//     }
//     }
//     else { //Else x coord is <=0.0f
//     if (centriodToCorners[i].y > 0.0f) {
//     //Then vector is in quadrant 2
//     //Check to see if it is the longest
//     if (corners2D[3].Length() < centriodToCorners[i].Length()) {
//     corners2D[3].x = unorderedCornersArray[i].x;
//     corners2D[3].y = unorderedCornersArray[i].y;
//     //                    corners2D[3] = centriodToCorners[i];
//     //                    centriodToCorners[i].x = 0.0f;
//     //                    centriodToCorners[i].y = 0.0f;
//     cindx4 = i;
//     }
//     }
//     else {
//     //Then Vector is in quadrant 3
//     if (corners2D[2].Length() < centriodToCorners[i].Length()) {
//     corners2D[2].x = unorderedCornersArray[i].x;
//     corners2D[2].y = unorderedCornersArray[i].y;
//     //                    corners2D[2] = centriodToCorners[i];
//     //                    centriodToCorners[i].x = 0.0f;
//     //                    centriodToCorners[i].y = 0.0f;
//     }
//     cindx3 = i;
//     }
//     }
//     }
//
//     //AFter running this check, it looks like the problem is occuring when 0 and 2 are getting set to the same value or 1 and 3 are getting set to the same value. So I am going to do a direct, inefficent fix for this
//     //////DEBUG CHECK
//     ////Check to see if any of the two corners got set to eachother
//     //for (int i = 0; i < BOX_CORNERS; ++i) {
//     //   for (int j = i+1; j < BOX_CORNERS; ++j) {
//     //if (corners2D[i] == corners2D[j]) {
//     //std::cout << "\nCorners[" << i << "] == Corners[" << j << "]\n";
//     //std::cout << "Corners[" << i << "] = " << corners2D[i].x << ", " << corners2D[i].y << std::endl;
//     //std::cout << "Corners[" << j << "] = " << corners2D[j].x << ", " << corners2D[j].y << std::endl;
//     //corners2D[j].x *= -1.0f;
//     //corners2D[j].y *= -1.0f;
//     // }
//     //}
//     //}
//     //Check for pathological case where there are no vectors in 2 of the 4 quadrants
//     if (corners2D[0] == corners2D[2]) { //If two corners were set to the same value (chances are both are still 0 vector)
//     //Then assign them to the second longest corner in the other two quadrants (note that 1 is quadrant 4 and 3 is quadrant 2)  (confusing, I know...)
//     //Move in order, set corner 0 to second largest vector in corner 1's quadrant, and set corner 2 to second longest vec in corner 3's quadrant
//
//     //Do special process for corner 0:
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (unorderedCornersArray[i].x == corners2D[1].x && unorderedCornersArray[i].y == corners2D[1].y) {
//     //Delete the longest vector from corner 1's quadrant
//     unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//     centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//     }
//     } //Set corner 0 to the second longest vector in corner 1's quadrant
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (centriodToCorners[i].x > 0.0f) {
//     if (centriodToCorners[i].y < 0.0f) {
//     if (corners2D[0].Length() < centriodToCorners[i].Length()) {
//     corners2D[0].x = unorderedCornersArray[i].x;
//     corners2D[0].y = unorderedCornersArray[i].y;
//     cindx1 = i;
//     }
//     }
//     }
//     }
//
//     //Repeat similar special process for corner 2:
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (unorderedCornersArray[i].x == corners2D[3].x && unorderedCornersArray[i].y == corners2D[3].y) {
//     //Delete the longest vector from corner 3's quadrant
//     unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//     centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//     }
//     } //Set corner 2 to the second longest vector in corner 3's quadrant
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (centriodToCorners[i].x < 0.0f) {
//     if (centriodToCorners[i].y > 0.0f) {
//     if (corners2D[2].Length() < centriodToCorners[i].Length()) {
//     corners2D[2].x = unorderedCornersArray[i].x;
//     corners2D[2].y = unorderedCornersArray[i].y;
//     cindx3 = i;
//     }
//     }
//     }
//     } */
//    /*if (corners2D[0].x > 0.0f) {
//     corners2D[2] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
//     //loop through the array of unordered corners
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
//     unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//     centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//     }
//     }
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (centriodToCorners[i].x > 0.0f) {
//     if (centriodToCorners[i].y > 0.0f) {
//     if (corners2D[2].Length() < centriodToCorners[i].Length()) {
//     corners2D[2].x = unorderedCornersArray[i].x;
//     corners2D[2].y = unorderedCornersArray[i].y;
//     }
//     }
//     }
//     }
//     }
//     else {
//     corners2D[0] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
//     //loop through the array of unordered corners
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
//     unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//     centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//     }
//     }
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     if (centriodToCorners[i].x < 0.0f) {
//     if (centriodToCorners[i].y > 0.0f) {
//     if (corners2D[0].Length() < centriodToCorners[i].Length()) {
//     corners2D[0].x = unorderedCornersArray[i].x;
//     corners2D[0].y = unorderedCornersArray[i].y;
//     }
//     }
//     }
//     }
//     } */
//}
//
//
//
////This code below right here fixes the wrong problem. If two vectors are equal, it's because no longest vector existed within their quadrant
////    //This code will fix faulty boxes formed if two vectors are in the same quadrant
////    if (corners2D[0] == corners2D[2]) { //If two corners were set to the same value
////        //Figure out what quadrant this vector exists in (NOTE THAT FOR 0 and 2, Y will always be positive)
////        if (corners2D[0].x > 0.0f) {
////            corners2D[2] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
////            //loop through the array of unordered corners
////            for (int i = 0; i < CUBOID_CORNERS; ++i) {
////                if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
////                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
////                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
////                }
////            }
////            for (int i = 0; i < CUBOID_CORNERS; ++i) {
////                if (centriodToCorners[i].x > 0.0f) {
////                    if (centriodToCorners[i].y > 0.0f) {
////                        if (corners2D[2].Length() < centriodToCorners[i].Length()) {
////                            corners2D[2].x = unorderedCornersArray[i].x;
////                            corners2D[2].y = unorderedCornersArray[i].y;
////                        }
////                    }
////                }
////            }
////        }
////        else {
////            corners2D[0] = aiVector2D(0.0f, 0.0f); //Reset the duplicate vector
////            //loop through the array of unordered corners
////            for (int i = 0; i < CUBOID_CORNERS; ++i) {
////                if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
////                    unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
////                    centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
////                }
////            }
////            for (int i = 0; i < CUBOID_CORNERS; ++i) {
////                if (centriodToCorners[i].x < 0.0f) {
////                    if (centriodToCorners[i].y > 0.0f) {
////                        if (corners2D[0].Length() < centriodToCorners[i].Length()) {
////                            corners2D[0].x = unorderedCornersArray[i].x;
////                            corners2D[0].y = unorderedCornersArray[i].y;
////                        }
////                    }
////                }
////            }
////        }
////    }
//
//
//if (corners2D[1] == corners2D[3]) { //If other two corners were set to the same value (chances are both are still 0 vector)
//
//    //Do special process for corner 1:
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        if (unorderedCornersArray[i].x == corners2D[2].x && unorderedCornersArray[i].y == corners2D[2].y) {
//            //Delete the longest vector from corner 1's quadrant
//            unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//            centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//        }
//    } //Set corner 1 to the second longest vector in corner 2's quadrant
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        if (centriodToCorners[i].x < 0.0f) {
//            if (centriodToCorners[i].y < 0.0f) {
//                if (corners2D[1].Length() < centriodToCorners[i].Length()) {
//                    corners2D[1].x = unorderedCornersArray[i].x;
//                    corners2D[1].y = unorderedCornersArray[i].y;
//                    cindx2 = i;
//                }
//            }
//        }
//    }
//
//    //Repeat similar special process for corner 3:
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        if (unorderedCornersArray[i].x == corners2D[0].x && unorderedCornersArray[i].y == corners2D[0].y) {
//            //Delete the longest vector from corner 3's quadrant
//            unorderedCornersArray[i] = aiVector3D(0.0f, 0.0f, 0.0f); //Set this vector to be 0 vector
//            centriodToCorners[i] = aiVector2D(0.0f, 0.0f); //Set the centriod-to-corner vector to 0 vector as well
//        }
//    } //Set corner 3 to the second longest vector in corner 0's quadrant
//    for (int i = 0; i < CUBOID_CORNERS; ++i) {
//        if (centriodToCorners[i].x > 0.0f) {
//            if (centriodToCorners[i].y > 0.0f) {
//                if (corners2D[3].Length() < centriodToCorners[i].Length()) {
//                    corners2D[3].x = unorderedCornersArray[i].x;
//                    corners2D[3].y = unorderedCornersArray[i].y;
//                    cindx4 = i;
//                }
//            }
//        }
//    }
//}
//
////Need to now scale corners2D and put them around the objects midpoint
//corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[0].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[0].y)));
//corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[3].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[3].y)));
//corners2D[2] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[2].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[2].y)));
//corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[1].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[1].y)));
//
//
////Print the corner indexes that were selected
//if (printDebugMessages) {
//    std::cout << "\nCorners from the 8 corners that were selected (indexed from 0): " << cindx1 << " " << cindx2 << " " << cindx3 << " " << cindx4 << std::endl;
//    }
//
//
//    /*  This new way is really close, but not quite
//     //New Different way of getting the four corners
//     //Set all vectors in corners2D to 0
//     for (int i = 0; i < BOX_CORNERS; ++i) {
//     corners2D[i].x = 0.0f;
//     corners2D[i].y = 0.0f;
//     }
//
//     aiVector2D cornersProjectedOntoPlane[CUBOID_CORNERS];
//     for (int i = 0; i < CUBOID_CORNERS; i+=1) {
//     cornersProjectedOntoPlane[i].x = unorderedCornersArray[i].x;
//     cornersProjectedOntoPlane[i].y = unorderedCornersArray[i].y;
//     }
//     //Need to find the four largest vectors in each quadrant
//     //Loop through the corner array again
//     for (int i = 0; i < CUBOID_CORNERS; i+=1) {
//     if (cornersProjectedOntoPlane[i].x > 0.0f) {
//     if (cornersProjectedOntoPlane[i].y > 0.0f) {
//     //Then vector is in quadrant 1
//     //Check to see if it is the longest
//     if (corners2D[0].Length() < cornersProjectedOntoPlane[i].Length()) {
//     corners2D[0] = cornersProjectedOntoPlane[i];
//     }
//     }
//     else {
//     //Then Vector is in quadrant 4
//     if (corners2D[1].Length() < cornersProjectedOntoPlane[i].Length()) {
//     corners2D[1] = cornersProjectedOntoPlane[i];
//     }
//     }
//     }
//     else { //Else x coord is <=0.0f
//     if (cornersProjectedOntoPlane[i].y > 0.0f) {
//     //Then vector is in quadrant 2
//     //Check to see if it is the longest
//     if (corners2D[3].Length() < cornersProjectedOntoPlane[i].Length()) {
//     corners2D[3] = cornersProjectedOntoPlane[i];
//     }
//     }
//     else {
//     //Then Vector is in quadrant 3
//     if (corners2D[2].Length() < cornersProjectedOntoPlane[i].Length()) {
//     corners2D[2] = cornersProjectedOntoPlane[i];
//     }
//     }
//     }
//     }
//
//     //Need to now scale corners2D and put them around the objects midpoint
//     corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[0].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[0].y)));
//     corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[3].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[3].y)));
//     corners2D[2] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[2].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[2].y)));
//     corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners2D[1].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners2D[1].y)));
//
//     */
//
//
//    /*
//     //I think my collision box is being bound to the axeses here, so need to find
//     //a diffrernt way to construct the 4 extreme points
//     //Need to get the four points that represent the extremes along x and y axes
//     int largestXIndxPos, largestXIndxNeg, largestYIndxPos, largestYIndxNeg;
//     largestXIndxPos = largestXIndxNeg = largestYIndxPos = largestYIndxNeg = 0;
//
//     //Now loop through the array to find which vectors in the cornerArray contain these maximums
//     float maxXPos, maxXNeg, maxYPos, maxYNeg;
//     maxXPos = maxXNeg = maxYPos = maxYNeg = 0.0f;
//     for (int i = 0; i < CUBOID_CORNERS; ++i) {
//     //get most positive x
//     if (unorderedCornersArray[i].x > maxXPos) {
//     maxXPos = unorderedCornersArray[i].x;
//     largestXIndxPos = i;
//     }
//     //get most negative x
//     if (unorderedCornersArray[i].x < maxXNeg) {
//     maxXNeg = unorderedCornersArray[i].x;
//     largestXIndxNeg = i;
//     }
//     //get most positive y
//     if (unorderedCornersArray[i].y > maxYPos) {
//     maxYPos = unorderedCornersArray[i].y;
//     largestYIndxPos = i;
//     }
//     //get most negative y
//     if (unorderedCornersArray[i].y < maxYNeg) {
//     maxYNeg = unorderedCornersArray[i].y;
//     largestYIndxNeg = i;
//     }
//     this->corners3D[i] = unorderedCornersArray[i];
//     }
//
//     //Set corners2D array
//     corners2D[0] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxPos].x)), scale * (midpoint.y - (collisionBoxShrinkageFactor * corners3D[largestYIndxPos].y)));
//     corners2D[1] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y - (collisionBoxShrinkageFactor * corners3D[largestYIndxPos].y)));
//     corners2D[2] = aiVector2D(scale * (midpoint.x - (collisionBoxShrinkageFactor * corners3D[largestXIndxPos].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
//     corners2D[3] = aiVector2D(scale * (midpoint.x + (collisionBoxShrinkageFactor * corners3D[largestXIndxNeg].x)), scale * (midpoint.y + (collisionBoxShrinkageFactor * corners3D[largestYIndxNeg].y)));
//
//     // corner1 = aiVector2D(scale*(midpoint.x + (boxShrinkageFactor * maxXPos)), scale *  (midpoint.y - (boxShrinkageFactor * maxYPos)));
//     //   corner2 = aiVector2D(scale * (midpoint.x + (boxShrinkageFactor * maxXNeg)), scale * (midpoint.y - (boxShrinkageFactor * maxYNeg))); */
//
//}

