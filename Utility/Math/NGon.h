//  This is a class for generating polygons. Can be used to construct any polygon (shape with 3 or more sides, with each side of equal length)
//  Functionality has been included to increment/decrement the number of sides on the polygon
//
//  NGon.h
//  SpaceShipGame3D
//
//  Created by Forrest Miller on 2/6/18.
//

// NOT FINISHED, BUT FUNCTIONAL

#ifndef NGon_h
#define NGon_h

#include <assimp/Importer.hpp> //Gives access to the aiVector3D

class NGon {
    //Position 0 is always set at straight up from position
private:
    //int numPositions, sides; //Sides == numPositions, so don't need both
    int sides;
    float size;
    aiVector3D orgin; //Position of the center
    aiVector3D ** sidePositions;
    float theta;
    
    void rotate();
    void computeOffsets(); //Compute offsets creates the aiVector3D objects
    
public:
    NGon() = delete;
    NGon(int numSides);
    NGon(int numSides, float theta); 
    NGon(int numSides, float theta, float size);
    NGon(int numSides, float theta, float size, aiVector3D startingPosition);
    NGon(const NGon &);
    //Destructor
    ~NGon();
    
    void changeTheta(float theta); //void rotate(float theta);
    void changePosition(aiVector2D); //move x y position
    void changePosition(aiVector3D); //move x y z position
    void changeSize(float size) {this->resize(size);} //Does exact same thing as resize;
    void resize(float size);
    void decreaseNumberOfSides();
    void increaseNumberOfSides();
    
    
    aiVector3D getOrgin() const {return this->orgin;}
    aiVector3D getMidpoint() const {return this->orgin;}
    aiVector3D ** getOffsets() const {return this->sidePositions;}
    float getTheta() const {return this->theta;}
    float getSize() const {return this->size;}
    int getNumSides() const {return this->sides;}
    //float * connectWithNext(const NGon &, int &); //Will create a new array of vertices connecting this NGon to another one
    //void connectWithNext(const NGon &, float *, int start, int finish); //Not sure what I am doing anymore...
    
};

#endif /* NGon_h */
