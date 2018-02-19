//  Abstract Base Class for a user-controlled player
//  Player.h
//
//  Created by Forrest Miller on 2/13/18.
//

#ifndef Player_h
#define Player_h

#include <stdio.h>
#include "Object.h"


class Player /*: public Object */ {
public:
    
private:
    //Shader * necessaryShader1, *necessaryShader2... , etc.
    //             (look into move semantics, so maybe then can build all shaders at once in a class then
    //              distribute them to every object that needs one
    
};

#endif /* Player_h */


//Just in case I forgot what an abstract base class looks like:
//
//  Shape.hpp
//  Tetris_Using_NCurses
//
//  This is the abstract base class Shape from which all other
//  Shape objects inherit from.
//
//  Created by Forrest Miller on 12/12/17.
//  Copyright © 2017 Forrest Miller. All rights reserved.
//

//#define POS0 0 //base
//#define POS1 1 //Rotate right 1 or left 3
//#define POS2 2 //Rotate right 2 or left 2
//#define POS3 3 //Rotate right 3 or left 1
//
//class Shape {
//private:
//    int colorPair;
//protected:
//    int size;
//    int xOrgin;
//    int yOrgin;
//    int orientation;
//    int previousOrientation;
//    //Rotation helper functions:
//    virtual void translateOrgin() = 0; //To make sure the shape stays in
//    //the same general area when rotating, make sure cycling through all 4 positions in order returns the orgin to it's starting position (i.e. the sum of all the translations results in 0) and also make sure each translation has it's counterpart as an inverse (i.e make sure that translation from 1->2 and 2->1 are inverse)
//    virtual void computeNewOffsets() = 0;
//public:
//    //Fields
//    int * xCoorOffsets;
//    int * yCoorOffsets;
//    //Constructors
//    Shape(int colPair);
//    Shape(int colPair, int orginY, int orginX);
//    //Destructor
//    //virtual ~Shape() = 0; //Not sure why this doesnt work: http://www.geeksforgeeks.org/pure-virtual-destructor-c/
//    virtual ~Shape() {;} //This works though
//
//    //Member Functions
//    void setOrgin(int y, int x);
//    int getOrginX() {return this->xOrgin;}
//    int getOrginY() {return this->yOrgin;}
//    void moveFromGravity(int amount); //Moves in the y direction 'amount' amount
//    void moveShapeUpOne();
//
//    void moveLeft(int amount) {this->xOrgin -= amount;}
//    void moveRight(int amount) {this->xOrgin += amount;}
//
//    virtual void rotateLeft() = 0;
//    virtual void rotateRight() = 0;
//    int getOrientation() {return this->orientation;}
//
//    int getColorPair() {return this->colorPair;}
//    int getSize() {return this->size;}
//    virtual int getHeight() = 0;
//    virtual int getWidth() = 0;
//
//};

