//  See page 248 of the 13th edition of 'Hibbeler Engineering Mechanics: Dynamics' textbook
//  Also see page 521 for eccentric impact
//  Impact2D.h
//
//  I haven't written this yet, but chances are this will wind up as a class that really could
//  have just been a function. Depends on how fancy I want to get.
//
//  Created by Forrest Miller on 3/9/18.
//
//  NOTE: I AM NOT ACTUALLY CLOSELY FOLLOWING REAL PHYSICS WITH THIS IMPLEMENTATION.
//
//  Since the spaceships are able to turn without using boosters, I am thinking I am
//  going to say that each ship has a giro-rotation-stablizer core, which keeps it stable
//  from picking up unwanted moments, which is also why no rotations happen when there
//  are collisions between ships or between ships and other objects.
//  Here is some moment-of-initeria links anyways:
//  see: https://www.c-motion.com/v3dwiki/index.php/MODEL_MOMENT_OF_INERTIA
//  see also: https://en.wikipedia.org/wiki/Moment_of_inertia
//  see also: https://en.wikipedia.org/wiki/List_of_moments_of_inertia
//
//

// Steps to calculating a collision:
//     1) Find the Plane of contact. Sources I consulted say that this needs to be found before the impact is actually
//        detected, but I can probably fudge it a bit
//     2) Find the line of impact. This will be perpendicular to the plane of impact
//     3) Swap velocities along the line of impact. Any motion perpendicular to the line of impact will be unaffected.
//        The mass of each object will also factor in here to how collision will be calculated.
//
//


//---------------------------------------------------------------------------
//    Idea for a better impact2D class:
//    Have a class called physicsInfo that can be attached to instances
//    Either within physicsInfo or seperate from physicsInfo, have collision detection
//    implemented as a collection of functions that take parameters such as:
//
//
//      (In InstanceManager):
//       doCollisionDetection() {
//          if (collisionDetected) {
//              seperate(); //i.e. call a MoveApartBy____() function on collisionBoxes and move midpoints with colBoxes
//              doImpact(const Instance.physicsInfo &, instance.velocity &, const instance.physicsInfo &, instance.velocity &);
//          }
//       }



#ifndef Impact2D_h
#define Impact2D_h

#include <iostream>
#include <math.h>
#include <assimp/Importer.hpp>
#include "GameParameters.h"
#include "Instance.h"
#include "CollisionBox.h"

class Impact2D {
private:
    float coefRest; //coefficientOfRestitution  (this is ratio of difference between velocities before collision vs ratio after)
    float m1, m2;
    
    void checkIfMassIsZero();
public:
    Impact2D();
    Impact2D(float coefficientOfRestitution);
    Impact2D(float m1, float m2);
    Impact2D(float coefficientOfRestitution, float m1, float m2);
    //Impact2D(Instance & i1, Instance & i2); Maybe have a function that takes instances as input, don't need to construct a new type of this object for each potential collision that could occur
    ~Impact2D();
    
    void computeCollisionSimple(aiVector2D & v1, aiVector2D & v2) const; //This just swaps the two objects velocities and reduces them by a small amount
    void computeCollisionSimple(aiVector2D & v1, aiVector2D & v2, const float& vMagnitudeChangeFactor) const;
    void computeCollision(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2) const;
    void computeCollisionAdvance(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2, const aiVector2D & pointOfImpact) const;
    //void computeEccentricImpactCollision( midpoints, velocitys, lineOfImpact, MomentsOfIntertia, more vars?);
    
    /**
     / Please do not call with mass = 0.0f;
     */
    void computeCollisionExperimental(aiVector2D & va1, aiVector2D & vb1, const float & massA, const float & massB, const CollisionBox & cboxA, const CollisionBox & cboxB, float coeffOfRest) const;
    void computeCollisionExperimentalWithEndingVeloctiyOutput(aiVector2D & va1, aiVector2D & vb1, const float & massA, const float & massB, const CollisionBox & cboxA, const CollisionBox & cboxB, float coeffOfRest) const;
    
    float getCoefficientRestitution() const {return this->coefRest;}
    void setCoefficientRestitution(float coefficientRest) {this->coefRest = coefficientRest;}
    
    float getMass1() const {return this->m1;}
    float getMass2() const {return this->m2;}
    void setMass1(float mass) {this->m1 = mass; checkIfMassIsZero();}
    void setMass2(float mass) {this->m2 = mass; checkIfMassIsZero();}
    
    
};

#endif /* Impact2D_h */
