//  NGon.cpp
//  SpaceShipGame3D
//
//  Created by Forrest Miller on 2/6/18.
//

#include "NGon.h"

const float PI = 3.1415926535f;

//class NGon {
//    //Position 0 is always set at straight up from position
//private:
//    //int numPositions, sides; //sides == numpositions
//    int sides;
//    int size;
//    aiVector3D position; //Position of the center
//    aiVector3D ** sidePositions;
//    float theta;
//    
//public:
//    NGon() = delete;
//    NGon(int numSides);
//    NGon(int numSides, int size);
//    NGon(const NGon &);
//    //Destructor
//    ~NGon();
//    
//    void changeTheta(float theta); //void rotate(float theta);
//    void changePosition(aiVector2D);
//    void changePosition(aiVector3D);
//    
//    int getNumSides() {return this->sides;}
//    aiVector3D ** getSidePositions() {return this->sidePositions;}
//    float getTheta() {return this->theta;}
//    
//    //float * connectWithNext(const NGon &, int &); //Will create a new array of vertices connecting this NGon to another one
//    //void connectWithNext(const NGon &, float *, int start, int finish); //Not sure what I am doing anymore...
//    
//};


NGon::NGon(int numSides) {
    this->sides = numSides;
    this->size = 1.0f; //i.e. radius
    if (numSides < 3) {this->sides = 3;} //need to have at least 3 sides because MATH!
    this->theta = 0.0f; //start unrotated
    //Set the side positions
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets(); //Compute the vertices of the NGon
}

NGon::NGon(int numSides, float size) {
    this->sides = numSides;
    this->size = size;
    if (numSides < 3) {this->sides = 3;}
    this->theta = 0.0f;
    //Create the array for the sidePositions
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets(); //Compute the vertices of the NGon
}


NGon::NGon(int numSides, float size, float theta) {
    this->sides = numSides;
    this->size = size;
    if (numSides < 3) {this->sides = 3;}
    this->theta = theta;
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets(); //Compute the offsets
}

NGon::NGon(int numSides, float size, float theta, aiVector3D startingPosition) {
    //Construct NGon as usual
    this->sides = numSides;
    this->size = size;
    if (numSides < 3) {this->sides = 3;}
    this->theta = 0.0f;
    //Set the side positions
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets();
    
    //Once constructed, then just call translate function with startingPosition
    this->changePosition(startingPosition);
}

NGon::NGon(const NGon & other) {
    
    
    
}

NGon::~NGon() {
    for (int i = 0; i < this->sides; ++i) {
        if (this->sidePositions[i] != nullptr) {
            delete this->sidePositions[i];
            this->sidePositions[i] = nullptr;
        }
    }
    if (this->sidePositions != nullptr) {
        delete [] this->sidePositions;
    }
}

void NGon::changeTheta(float theta) { //Make it so have function to change rotation by an amount, instead of always specifing a theta value
    this->theta = theta;
    rotate();
}

void NGon::rotate() { //Uses the theta stored within this object
    aiMatrix3x3 rotationMatrixZAxis ( cos(this->theta), -sin(this->theta), 0.0f,
                                     sin(this->theta),  cos(this->theta), 0.0f,
                                     0.0f,               0.0f,      1.0f);
    
    for (int i = 0; i < this->sides; ++i) {
        // | dereference pointer               multiply |    | dereference ptr
        // V                                            V    V
        (  *(sidePositions[i]) ) = rotationMatrixZAxis  *   (*(sidePositions[i]));
        // ^                                            ^    ^
        // | dereference pointer               multiply |    | dereference ptr
    }
}

void NGon::decreaseNumberOfSides() {
    if (this->sides <= 3) {return;} //Don't shrink below 3
    //Might want to preserve current rotation on NGon? Not sure how yet, figure out later once theta is figured out
    for (int i = 0; i < this->sides; ++i) {
        if (this->sidePositions[i] != nullptr) {
            delete this->sidePositions[i];
            this->sidePositions[i] = nullptr;
        }
    }
    if (this->sidePositions != nullptr) {
        delete [] this->sidePositions;
    }
    this->sides -= 1;
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets();
    rotate();
}

void NGon::increaseNumberOfSides() {
    //Might want to preserve current rotation on NGon? Not sure how yet, figure out later once theta is figured out
    for (int i = 0; i < this->sides; ++i) {
        if (this->sidePositions[i] != nullptr) {
            delete this->sidePositions[i];
            this->sidePositions[i] = nullptr;
        }
    }
    if (this->sidePositions != nullptr) {
        delete [] this->sidePositions;
    }
    this->sides += 1;
    this->sidePositions = new aiVector3D* [this->sides];
    computeOffsets();
    rotate();
}

void NGon::changePosition(aiVector2D transl) { //move x y position
    this->orgin.x = transl.x; //Was +=
    this->orgin.y = transl.y; //Was +=
    //aiVector3D tmpTransl(transl.x, transl.y, 0.0f); //need to convert vec2 to vec3
    for (int i = 0; i < this->sides; ++i) {
        (this->sidePositions[i])->x += transl.x;
        (this->sidePositions[i])->y += transl.y;
    }
    resize(this->size); //Calling resize causes the offset positions to be recalculated from the orgin in it's new position
    
    rotate(); //Might not need to 'rotate' here. Oh well...
    
//    for (int i = 0; i < this->sides; ++i) {
//        if (this->sidePositions[i] != nullptr) {
//            delete this->sidePositions[i];
//            this->sidePositions[i] = nullptr;
//        }
//    }
//    //Compute new offsets (probably don't need to delete all the offsets, but oh well
//    computeOffsets();
}
void NGon::changePosition(aiVector3D transl) { //move x y z position
    this->orgin.x = transl.x; //was +=
    this->orgin.y = transl.y;
    this->orgin.z = transl.z;
    for (int i = 0; i < this->sides; ++i) {
        *(this->sidePositions[i]) += transl;
    }
    
    resize(this->size); //Calling resize causes all the sidePositions to be recomputed
    
    rotate();
//    for (int i = 0; i < this->sides; ++i) {
//        if (this->sidePositions[i] != nullptr) {
//            delete this->sidePositions[i];
//            this->sidePositions[i] = nullptr;
//        }
//    }
//    //Compute new offsets (probably don't need to delete all the offsets, but oh well
//    computeOffsets();
}

void NGon::resize(float size) {
    this->size = size;
    //need to set new offsets
    float angle;
//
//    if (this->sidePositions[0] == nullptr) {
//        computeOffsets(); //This really should have been done before this function is ever called
//    }
    
    this->sidePositions[0]->x = 0.0f;
    this->sidePositions[0]->y = this->size;
    this->sidePositions[0]->z = 0.0f;
    
    for (int i = 1; i < this->sides; ++i) {
        //angle = ((float)i / (float) this->sides * 2.0f*PI) + (PI / 2.0f);
        angle = ((2.0f * PI) * i / this->sides) + (PI / 2);
        //angle = (2.0f * PI) / ((float)i);
        //float angle = float(i) * PI * 2.0f / (float)this->sides;
//        this->sidePositions[i] = new aiVector3D(this->size*cos(angle), this->size * sin(angle) , 0.0f);
        this->sidePositions[i]->x = this->size*cos(angle);
        this->sidePositions[i]->y = this->size*sin(angle);
        this->sidePositions[i]->z = 0.0f;
    }
    //Retranslate the newly set offsets
    for (int i = 0; i < this->sides; ++i) {
        *(this->sidePositions[i]) += this->getMidpoint();
    }
}

//Private function that should be called from the constructors
void NGon:: computeOffsets() {
    //Turns out 3 sides is not a special case
//    if (this->sides == 3) { //Then shape is a triangle
//        //135 degree angles between each point
//        this->sidePositions[0] = new aiVector3D(cos(0.5f * PI), sin(0.5f * PI), 0.0f);
//       // this->sidePositions[1] = new aiVector3D(cos(1.25 * PI), sin(1.25 * PI), 0.0f);
//        this->sidePositions[1] = new aiVector3D(cos(1.166666666f * PI), sin(1.166666666f * PI), 0.0f); //1.25?
//        this->sidePositions[2] = new aiVector3D(cos(1.861111111f * PI), sin(1.861111111f * PI), 0.0f);
//       // this->sidePositions[2] = new aiVector3D(cos(2.0 * PI), sin(2.0 * PI), 0.0f);
//        //Wrong (but still a triangle):
//        // this->sidePositions[0] = new aiVector3D(0.0f, 1.0f, 0.0f);
//        //this->sidePositions[1] = new aiVector3D(cos(7.0f*PI/4.0f), sin(7.0f*PI/4.0f), 0.0f);
//        //this->sidePositions[2] = new aiVector3D(cos(5.0f*PI/4.0f), sin(5.0f*PI/4.0f), 0.0f);
//    }
    //else {
    float angle;
    this->sidePositions[0] = new aiVector3D(0.0f, this->size, 0.0f);
    for (int i = 1; i < this->sides; ++i) {
        angle = ((2.0f * PI) * i / this->getNumSides()) + (PI / 2);
    //float angle = float(i) * PI * 2.0f / (float)this->sides;
        this->sidePositions[i] = new aiVector3D(this->size*cos(angle), this->size * sin(angle), 0.0f);
    }
   // }
    //Retranslate the newly set offsets
    for (int i = 0; i < this->sides; ++i) {
        *(this->sidePositions[i]) += this->getMidpoint();
    }
}
