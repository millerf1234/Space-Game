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
//
//  see also: https://en.wikipedia.org/wiki/Moment_of_inertia
//  see also: https://en.wikipedia.org/wiki/List_of_moments_of_inertia

#ifndef Impact2D_h
#define Impact2D_h

#include <iostream>
#include <assimp/Importer.hpp>
#include "GameParameters.h"

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
    ~Impact2D();
    
    void computeCollisionSimple(aiVector2D & v1, aiVector2D & v2) const;
    void computeCollision(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2) const;
    void computeCollisionAdvance(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2, const aiVector2D & pointOfImpact) const;
    //void computeEccentricImpactCollision( midpoints, velocitys, lineOfImpact, MomentsOfIntertia, more vars?);
    
    float getCoefficientRestitution() const {return this->coefRest;}
    void setCoefficientRestitution(float coefficientRest) {this->coefRest = coefficientRest;}
    
    float getMass1() const {return this->m1;}
    float getMass2() const {return this->m2;}
    void setMass1(float mass) {this->m1 = mass; checkIfMassIsZero();}
    void setMass2(float mass) {this->m2 = mass; checkIfMassIsZero();}
    
    
};

#endif /* Impact2D_h */
