//  This class is very similar to the CollisionRectangle class (in fact they
//  share probably around 90% of the same code), except this class bounds
//  models more tightly by being axis-aligned.
//
//  This class is intended for processing collsions for a 3D model that is
//  restricted to always exist within a 2D plane. The idea is for the box to always
//  encompass the 3D object's screenspace, even as the 3D object undergoes rotations
//  in 3D.
//
//  --To Add Eventually: Make it so that the 2D plane can be set to be any arbitrary
//    plane, not just the xy-axis (i.e. the z=0 plane)
//
//  AACollisionBox.h
//
//  Created by Forrest Miller on 3/5/18.
//

//One other thing to do eventually -- Have this collisionBox be able to test
//itself with other collision detection objects


#ifndef AACollisionBox_h
#define AACollisionBox_h

#include <iostream>
#include <vector>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D (and 2D)

#include "Quaternion.h"//Use the Quaternion class I wrote
#include "GameParameters.h" //Needed to get the constant COLLISION_SAMPLE_POINTS

//Important Note on accurate box generation:
//This class is intended to be set from a model that is oriented in the way that
//will maximize x, y and z values. In other words, rotating the model should
//ideally not cause x, y, or z to grow larger than the values they have when
//the model data is provided to this object. This object calculates values in
//both directions along the axis and will store both direction values if the
//model is not symmetrical, so objects need not be symmetrical.
//
// NOTE: THIS CLASS ASSUMES THAT THE ORGIN IN THE COORDINATE DATA IS LOCATED
//       SOMEWHERE INSIDE THE MODEL. HAVING A MODEL THAT HAS THE ORGIN AS AN
//       EXTERNAL POINT WILL RESULT IN AN INACCURATE COLLISION BOX BEING FORMED!
//
//  to add --
//(In the future this class could be modified to calculate moment-of-intertia of
//the model and then from that get the principle axes [which I think would could
//be used to create the proper collision rectangle for a model in any
//orientation] )
//

//----------------------------------------------------------------------------
//                          !!!IMPORTANT!!!
//USAGE INSTRUCTIONS: 1) Currently there are three different constructors for
//                       creating a new AACollisionBox object:
//
//                      (i)   Construct the box with no parameters. If
//                            this constructor is used, see step 2.
//
//                      (ii)  Construct with an array of floats representing
//                            the x,y,z coordinates of the model. Position
//                            data would be in the same format as OpenGL
//                            expects when using the drawArrays command
//                            (i.e. x0 y0 z0 x1 y1 z1 x2 y2 z2 ... )
//                            IMPORTANT! If using this constructor, the
//                            array size must be a multiple of 3 (using an
//                            array of size other than a multiple of 3 will
//                            cause object construction to default to
//                            constructor (i).
//
//                      (iii) Construct with an array of 3D vectors
//                            representing the position data.
//
//                      If using constructors (ii) or (iii), skip to step 3.
//
//                   2) If used parameterless construcor, call the function
//                      setFromModelData to have this object configure itself
//                      to match the model. Note that doing this will cause
//                      any stored rotations to be deleted, meaning the
//                      rotationOrder will need to be repopulated to follow
//                      rotations the model is undergoing.
//
//                   3) Set the midpoint of this to match the midpoint of the
//                      model. It is VERY IMPORTANT to do this step or the
//                      collision box will just be bound to the orgin.
//
//                   4) Add any rotations to the rotationOrder array of
//                      quaternions. If many rotations are to be added, it is
//                      possible to pre-specificy an arraySize for the array to
//                      prevent it from having to be resized each time a new
//                      rotation is added. Each new rotation that is added will
//                      cause this rectangle to recompute itself accordingly.
//                      Once in place, both the rotation angle and axis can be
//                      modified using functions, with the intents that any
//                      object that has a rotation box will use these functions
//                      to give the collisionRectangle guidance on how to keep
//                      itself update with the models position/orientation.
//
//                    That is all there is to get an AACollisionBox set up.
//                    Note that these Boxes (cuboids) are built from 3D models,
//                    but their collision-detection functions only do collision
//                    detection within a 2D plane.
//
//----------------------------------------------------------------------------


class CollisionBox {
private:
    //Private fields (variables)
    bool printDebugMessages;
    bool printDebugWarnings;
    bool hasModelData;
    bool xAxisSymmetry, yAxisSymmetry, zAxisSymmetry;

    Quaternion ** rotationOrder; //Rotations to be performed, should match rotations on model
    Quaternion hiddenRotation; //Used to fudge the collision rectangle to give max/min x-y values
    bool useHiddenRotation; //Used to determine whether to fudge the collision rectangle
    
    int numberOfRotations; //Number of rotations in the array
    int rotationOrderSize; //The size allocated for the array

    //Some private helper functions
    void buildCornerAdjacencyList(); //Call this once after model data set/reset
    void doRotationsAndRecalculate(); //Used to recalculate box after a rotation is updated
    void setCorners3D(); //Called by doRotationsAndRecalculate()
    void calculateSelfAfterTranslations();

    //Initialization function for consructors to call
    void initialize();
    
    //Adding this to try to get box projection fixed
    int adjacencyList[8][3]; //An Array containing the adjacent indices to each corner
    
protected:
    float zPlane2D, zMidpointOffset3D;
    //If the model is larger in one direction that in it's opposite, the longer
    //axis becomes the major and the shorter axis becomes the minor.
    aiVector3D xAxisMajor, yAxisMajor, zAxisMajor, xAxisMinor, yAxisMinor, zAxisMinor;
    aiVector3D originalMajorsFromModel, originalMinorsFromModel;
    aiVector3D corners3D[8]; //The 8 3D coordinates of the box corners after rotation
    aiVector2D midpoint;
    aiVector2D corners2D[4]; //The four corners of the box within the collision plane

    //For printing collision detection traces
    std::vector<float> * collisionSamplePointsAll;
    std::vector<float> * collisionSamplePointsThis;
    
    //Scale should be set to 1/w to match model's w so box scales with model
    float scale;
    //Since the box is being set from the model extremes, this can be used to move the box inside the model
    float collisionBoxShrinkageFactor;

    //Protected function for checking to see if the 2D box has area
    bool hasNoArea() const;
    
    //Get the largest absolute value in an array
    float getMaxFromArray(float * data, int dataSize);
    //Gets the most positive value in an array of floats
    float getMaxFromArrayPositiveOnly(float * data, int dataSize);
    //Gets the most negative value in an array of floats
    float getMaxFromArrayNegativeOnly(float * data, int dataSize);
    
    //Here are some math functions that are used:
    //Need a dot product operation
    float dot(const aiVector3D & v1, const aiVector3D & v2) {
        return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
    }
    //Need a function to get the vector between two points
    aiVector2D getVectorBetween(const aiVector2D& head, const aiVector2D& tail) const {
        return aiVector2D(head.x-tail.x, head.y-tail.y);
    }
    
    float getDistance(const aiVector2D & p1, const aiVector2D & p2) const {
        return (getVectorBetween(p1, p2).Length());
    }
    aiVector2D getMidpoint(const aiVector2D & p1, const aiVector2D & p2) const {
        aiVector2D midpointDisplacement = getVectorBetween(p2, p1); //p2 head, p1 tail
        midpointDisplacement *= 0.5f; //Shorten the vector by half
        return (p1 + midpointDisplacement);
    }
    
    float getTriangleArea(const aiVector2D& p0, const aiVector2D& p1, const aiVector2D& p2) const {
        return abs(((-p1.x*p0.y + p2.x*p0.y + p0.x*p1.y - p2.x*p1.y - p0.x*p2.y + p1.x*p2.y) / 2.0f));
        //(-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2); //Computation to be performed (basically just a determinant)
    }
    
public:
    //--------------------------------------------------------------------------
    //                      Construction/Initialization
    //--------------------------------------------------------------------------
    //Constructors
    CollisionBox();
    CollisionBox(float * data, int dataPoints); //dataPoints must be a multiple of 3
    CollisionBox(aiVector3D *, int numberOfVectors);
    CollisionBox(const CollisionBox &) = delete;//No copy constructor

    //Destructor
    ~CollisionBox();

    //No copy assignment operator
    CollisionBox& operator=(const CollisionBox &) = delete;

    void setFromModelData(float * data, int dataPoints); //Will cause rotationOrder to be cleared
    bool getHasModelData() const {return hasModelData;} //Check to see if box construction successful

    //Midpoint getter and setter (which include function to translate rectangle by a 2D vector)
    aiVector2D getMidpoint() const {return this->midpoint;}
    void translate(const aiVector2D & transl) {this->midpoint += transl; calculateSelfAfterTranslations();}
    void setMidpointTo(const aiVector2D & newMidpoint) {this->midpoint = newMidpoint; calculateSelfAfterTranslations();}

    //Size getters and setters
    float getScale() const {return this->scale;}
    void setScale(float newScale) {this->scale = newScale;}
    float getCollisionBoxShrinkageFactor() const {return this->collisionBoxShrinkageFactor;}
    void setCollisionBoxShrinkageFactor(float shrinkageFactor) {this->collisionBoxShrinkageFactor = shrinkageFactor;}

    //Rotation Functions:
    void presetRotationOrderSize(int size); //Allows for space to be allocated for all the rotations before adding them
    void addToRotationOrder(const Quaternion &);
    void changeRotationAt(int index, float theta);
    void changeRotitationAxisAt(int index, const Quaternion &);
    void changeRotitationAxisAt(int index, const aiVector3D &, float theta);
    void changeRotitationAxisAt(int index, float x, float y, float z, float theta);
    float getRotationThetaAt(int index);
    void clearRotationOrder();
    void removeRotationAtIndex(int index);
    int getNumberOfRotations() const {return this->numberOfRotations;}

    //--------------------------------------------------------------------------
    //              Collision Detection Functions
    //--------------------------------------------------------------------------
    
    float getQuadrilateralArea() const; //Gets the area of the collisionBox
    
    //Collision Detection Functions
    bool isWithin(float x, float y) const;
    bool isWithin(const aiVector2D & point) const;
    bool isOverlapping(const CollisionBox&) const;

    //CollisionBox Distance/Direction finding functions. Results are approximate
    float getClosestDistanceToBox(const CollisionBox & otherBox) const;
    aiVector2D getClosestDirectionToBox(const CollisionBox & otherBox) const;
    float getClosestDistanceToPoint(const aiVector2D & point) const;
    aiVector2D getClosestDirectionToPoint(const aiVector2D & point) const;
    float getClosestDistanceToPoint(const float & x, const float & y) const;
    aiVector2D getClosestDirectionTo(const float & x, const float & y);
    //TODO Add more functions here once more Collision Detection shape classes are in place

    //CollisionBox Distance/Direction finding functions using boxes midpoint (instead of edges)
    float getDistanceBetweenMidpoints(const CollisionBox & otherBox) const;     //NEEDS TO BE IMPLEMENTED!
    aiVector2D getVectorBetweenMidpoints (const CollisionBox & otherBox) const; //NEEDS TO BE IMPLEMENTED
    float getDistanceFromMidpointToPoint(const aiVector2D & point) const; //NEEDS TO BE IMPLEMENTED
    aiVector2D getVectorFromMidpointToPoint(const aiVector2D & point) const; //NEEDS TO BE IMPLEMENTED
    float getDistanceFromMidpointToPoint(const float & x, const float & y) const; //NEEDS TO BE IMPLEMENTED
    aiVector2D getVectorFromMidpointToPoint(float x, float y) const; //NEEDS TO BE IMPLEMENTED

    //Function to facilitate seperating objects if their boxes overlap
    void moveApartAlongAxisBetweenClosestDetectedPoints(CollisionBox & other);
    void moveApartAlongAxisBetweenMidpoints(CollisionBox &);
    void moveApartAlongAxisBetweenMidpointsThisOnly(CollisionBox &);
    void moveApartAlongAxisBetweenMidpointsOtherOnly(CollisionBox &);


    //--------------------------------------------------------------------------
    //                 Box Position Coordinate Functions (for drawing)
    //--------------------------------------------------------------------------
    void setZPlaneValueForRect2D(float zValue); //Used for changing the zPlane
    void setZMidpointValueForCuboid3D(float zValue); //Used for translating 3D box midpoint in z direction
    
    void getRotatedMajorMinor3D(float * bufferOfThirtysixFloats) const;
    //Functions to help faciliate drawing the collision box
    void getRect2DCornerPoints2D(float * bufferOfEightFloats) const; //sets x,y values
    void getRect2DCornerPoints3D(float * bufferOfTwelveFloats) const; //sets x,y,z values
    
    void getCubiodTriangles3D(float * bufferOf108Floats) const;//(NOTE: HAVE TESTed THIS ONE, IT WORKS WELL. DONT CHANGE!)
    //These next two return formated position values for drawing various primatives
    void getRectCornersTriangles3D(float * bufferOfEighteenFloats) const; //sets x,y,z values
    void getRectCornersLines3D(float * bufferOfTwentyfourFloats) const; //sets x,y,z values
    
    //Gets the Sample points most recently used in collision detection
    void resetCollisionDetectionSamplePoints();
    void getCollisionDetectionSamplePointsBoxMidpointToBoxMidpoint(float * bufferOfFourFloats, const CollisionBox &) const;
    std::vector<float>* getCollisionDetectionSamplePoints() const;
    std::vector<float>* getCollisionDetectionSamplePointsThisOnly() const;
    
    
    //TODO Add more getters once different collision detection geometry is in place

    
    //--------------------------------------------------------------------------
    //                Debug Message Activation/Deactivation
    //--------------------------------------------------------------------------
    void turnOnDebugMessages() {this->printDebugMessages = true;}
    void turnOffDebugMessages() {this->printDebugMessages = false;}
    void turnOnDebugWarnings() {this->printDebugWarnings = true;}
    void turnOffDebugWarnings() {this->printDebugWarnings = true;}
};
#endif /* AACollisionBox_h */

