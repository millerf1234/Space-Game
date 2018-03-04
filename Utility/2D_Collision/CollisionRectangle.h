//  This is a rectangle I am using to do 2D collision with my Player Instances
//  (and maybe other instances if it turns out to be portable)
//
//  This class is intended for processing collsions for a 3D model that is
//  restricted to always exist within a 2D plane. The idea is for the box to always
//  encompass the 3D object's screenspace, even as the 3D object undergoes rotations
//  in 3D.
//
//  CollisionRectangle.h
//
//  Created by Forrest Miller on 2/28/18.
//

#ifndef CollisionRectangle_h
#define CollisionRectangle_h


#include <iostream>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D

#include "Quaternion.h"//Use the Quaternion class I wrote


//This class is intended to be set from a model that is oriented in the way that
//will maximize x, y and z values. In other words, rotating should ideally not
//cause x, y, or z to grow larger than the largest of x, y, z when the value are
//calculated from the model.
//(In the future this class could be modified to calculate moment-of-intertia of
//the model and then from that get the principle axes [which I think would could
//be used to create the proper collision rectangle for a model in any
//orientation] )

//----------------------------------------------------------------------------
//                          !!!IMPORTANT!!!
//  USAGE INSTRUCTIONS: 1) Currently there are three different constructors for
//                     creating a new CollisionRectangle object:
//
//                          (i)   Construct the rectangle with no parameters. If
//                                this constructor is used, see step 2.
//
//                          (ii)  Construct with an array of floats representing
//                                the x,y,z coordinates of the model. Position
//                                data would be in the same format as OpenGL
//                                expects when using the drawArrays command
//                                (i.e. x0 y0 z0 x1 y1 z1 x2 y2 z2 ... )
//                                IMPORTANT! If using this constructor, the
//                                array size must be a multiple of 3 (using an
//                                array of size other than a multiple of 3 will
//                                cause object construction to default to
//                                constructor (i).
//
//                          (iii) Construct with an array of 3D vectors
//                                representing the position data.
//
//                        If using constructors (ii) or (iii), skip to step 3.
//
//                      2) If used parameterless construcor, call the function
//                        setFromModelData to have this object configure itself
//                        to match the model. Note that doing this will cause
//                        any stored rotations to be deleted, meaning the
//                        rotationOrder will need to be repopulated to follow
//                        rotations the model is undergoing.
//
//                     3) Set the midpoint of this to match the midpoint of the
//                        model. It is VERY IMPORTANT to do this step or the
//                        collision box will just be bound to the orgin.
//
//                     4) Add any rotations to the rotationOrder array of quaternions.
//                        If many rotations are to be added, it is possible to
//                        pre-specificy an arraySize for the array to prevent it
//                        from having to be resized each time a new rotation is
//                        added. Each new rotation that is added will cause this
//                        rectangle to recompute itself accordingly. Once in place,
//                        both the rotation angle and axis can be modified using functions,
//                        with the intents that any object that has a rotation box will
//                        use these functions to give the collisionRectangle guidance
//                        on how to keep itself update with the models position/orientation.
//
//
//                    That is all there is to get a collisionRectangle set up.
//                    Note that these Rectangles are built from 3D models, but
//                    only do collision detection in a 2D plane.
//
//
//----------------------------------------------------------------------------


//Could theoretically calculate the moment of initera and from that get a midpoint and get
//the principle axeses, but for now will not do things that way.
class CollisionRectangle {
private:
    float x, y, z; //Maximum model limits that will rotate when model rotates
    aiVector3D maxFromModelXYZ; //maxFromModelXYZ is set when box is given model data, and won't change beyond that
    aiVector2D corner1, corner2; //corners will change often with rotations and translations. This is world space coordinates, calculated from a midpoint that intended to be linked to a models midpoint.
    aiVector3D corner1OffsetFromMidpointBeforeRotations, corner2OffsetFromMidpointBeforeRotations; //This is just a description of the 3D collision box in model-coordinates (i.e. these are both just offsets from a midpoint located at the orgin)
   
    aiVector2D midpoint; //The center of the rectangle. Translations only need to move this midpoint, and the class will take care of recalculating the rectangle corners as needed.
    
    Quaternion ** rotationOrder;
    int numberOfRotations; //This is the number of actual quaternion rotations in the array
    int rotationOrderSize; //This is the size of rotationOrder array
    
    float scale; //Used to scale with the models
    
    bool hasModelData;
    
    void calculateSelfAfterTranslations(); //Calculate corner1 and corner2 from originalCorners and rotations
    //Call this next function after any changes are made to rotation
    void doRotationsAndRecalculate(); //Do rotations to get new values for x,y,z, then recalculate the corners
    void calculateSelfBeforeTransformations(); //Calculate corner1, corner2 and originalCorners from midpoint and xyz
    
    bool hasNoArea() const;//Checks to see if the collisionRectangle is taking up any area within the collision plane
    
    //Functions for getting box bounds from model data  (dataSize should be the size of the array)
    //Gets the largest absolute value in an array of floats
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
    aiVector2D getVectorBetween(const aiVector2D& head, const aiVector2D& tail) {
        return aiVector2D(head.x-tail.x, head.y-tail.y);
    }
public:
    CollisionRectangle();
    CollisionRectangle(float * data, int dataPoints);
    CollisionRectangle(aiVector3D *, int numberOfVectors);
    CollisionRectangle(const CollisionRectangle &) = delete;//No copy constructor
    
    //Destructor
    ~CollisionRectangle();
    
    //Call this function if default constructor was called
    void setFromModelData(float * data, int dataPoints);
    bool getHasModelData() const {return hasModelData;}
    
    //No copy assignment operator...
    CollisionRectangle& operator=(const CollisionRectangle &) = delete;
    
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
    int getNumberOfRotations() const;
    //void getRotationOrder(Quaternion *) const; //This function is dangerous, so not going to allow it to be used
    
    //Scale getter and setter
    float getScale() const {return this->scale;}
    void setScale(float newScale) {this->scale = newScale;}
    
    //Midpoint getter and setter (which include function to translate rectangle by a 2D vector)
    aiVector2D getMidpoint() const {return this->midpoint;}
    void translate(const aiVector2D & transl) {this->midpoint += transl; calculateSelfAfterTranslations();}
    void setMidpointTo(const aiVector2D & newMidpoint) {this->midpoint = newMidpoint; calculateSelfAfterTranslations();}
    
    //Collision detection functions
    bool isWithin(float x, float y);
    bool isWithin(const aiVector2D & point);
    bool isOverlapping(const CollisionRectangle&);
    
    //Distance Finding Functions
    /*!
     Get Distance(vector) between another rectangle and This rectangle (Approximate)
     @remark  This function works by sampling a number of points on each rectangle
     @remark  Will return 0 (or 0 vector) if one rectangle is inside the other
     */
    float getClosestDistanceTo_Scalar(const CollisionRectangle& otherRect) const;
    aiVector2D getClosestDistanceTo_Vector(const CollisionRectangle& otherRect) const;
    
    /*!
     Get Distance Between Another Rectangle and This Rectangle (Approximate)
     @remark  This is a brute-force function
     @remark  Precision is the number of point to check along each side of the rectangle
     @remark  Will return 0 if one rectangle is inside the other
     */
    float closestDistanceTo(const CollisionRectangle& otherRect, int precision) const;
    
    /*!
     Get approximatly the shortest distance between a point in space and a side of this rectangle
     @remark   Will return 0 if the point is inside the rectangle
     */
    float closestDistanceTo(const aiVector2D& point)const;
    
    //Gonna not use this function
    ///*!
    //Get approximatly the shortest distance between a point in space and a side of this rectangle
    //@remark   Will return 0 if the point is inside the rectangle
    //@remark  Precision is the number of point to check along each side of the rectangle
    //*/
    //float closestDistanceTo(const aiVector2D& point, int precision)const;
    
    /*!
     Get approximatly the shortest distance between a point in space and a side of this rectangle
     @remark   Will return 0 if the point is inside the rectangle
     */
    float closestDistanceTo(float x, float y) const;
    
    /*!
     Get approximatly the shortest distance between a point in space and a side of this rectangle
     @remark   Will return 0 if the point is inside the rectangle
     @remark  Precision is the number of point to check along each side of the rectangle
     */
    float closestDistanceTo(float x, float y, int presicison)const;
    
    float getDistanceBetweenMidpoints_Scalar(const CollisionRectangle& otherRect) const; //Self explanitory I hope
    aiVector2D getDistanceBetweenMidpoints_Vector(const CollisionRectangle& otherRect) const; //Returns a vector
    
    void moveApartAlongAxisBetweenClosestDetectedPoints(CollisionRectangle &);
    void moveApartAlongAxisBetweenMidpoints(CollisionRectangle &);
    void moveApartAlongAxisBetweenMidpointsThisOnly(CollisionRectangle &);
    void moveApartAlongAxisBetweenMidpointsOtherOnly(CollisionRectangle &);
    

    void getRectCornersPoints(float * bufferOfEightFloats) const;
    void getRectCornersTriangles(float * bufferOfTwelveFloats) const;
    void getRectCornersLines(float * bufferOfSixteenFloats) const;
    
    bool operator==(const CollisionRectangle & otherRect) const {
        return ((this->midpoint == otherRect.midpoint) && (this->corner1 == otherRect.corner1) && (this->corner2 == otherRect.corner2));
    }
    bool operator!=(const CollisionRectangle & otherRect) const {
        return (!((*this) == otherRect));
    }
};




#endif /* CollisionRectangle_h */
