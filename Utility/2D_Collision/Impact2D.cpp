//  ImplementationFile for the Impact2D class
//  Impact2D.cpp
//
//  Created by Forrest Miller on 3/9/18.
//

#include "Impact2D.h"

constexpr static float FP_TOLERANCE = 0.00001f;

Impact2D::Impact2D() {
    m1 = m2 = coefRest = 1.0f;
}

Impact2D::Impact2D(float coefficientOfRestitution) {
    this->m1 = this->m2 = 1.0f;
    if (coefficientOfRestitution < FP_TOLERANCE || coefficientOfRestitution > 1.0f) {
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! coefficientOfRestitution cannot be set to a value outside the range of [0.0f, 1.0f]\n";
        }
        coefRest = 1.0f;
        return;
    }
    coefRest = coefficientOfRestitution;
}

Impact2D::Impact2D(float m1, float m2) {
    this->m1 = m1;
    this->m2 = m2;
    checkIfMassIsZero();
    this->coefRest = 1.0f;
}

Impact2D::Impact2D(float coefficientOfRestitution, float m1, float m2) {
    //Set masses
    this->m1 = m1;
    this->m2 = m2;
    checkIfMassIsZero();
    //Set coefficientOfRestitution
    if (coefficientOfRestitution < FP_TOLERANCE || coefficientOfRestitution > 1.0f) {
        if (PRINT_DEBUG_WARNING_MESSAGES) {
            std::cout << "\nDEBUG::WARNING! coefficientOfRestitution cannot be set to a value outside the range of [0.0f, 1.0f]\n";
        }
        coefRest = 1.0f;
        return;
    }
    coefRest = coefficientOfRestitution;
}

Impact2D::~Impact2D() {
    //Nothing to destruct currently
}

void Impact2D::computeCollisionSimple(aiVector2D & v1, aiVector2D & v2) const {
    //Note that I am not using restitution correctly here, instead I just made it a scaling factor
    //If the masses are equal
    if (m1 == m2 || true) {
        aiVector2D temp = v1 * coefRest;
        v1 = v2 * coefRest;
        v2 = temp;
    }
//    } //If 1 has greater mass than 2
//    else if (m1 > m2) {
//        //set v1 by:
//
//        aiVector2D momentum2 = v2 * m2/m1;
//        v1 = (v1 + momentum2) * coefRest;
//        v2 = momentum2 + (v1 * coefRest);
//    }
//    else { //m1 < m2
//
//    }
}
void Impact2D::computeCollision(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2) const {
    
}
void Impact2D::computeCollisionAdvance(const aiVector2D & mid1, aiVector2D & v1, const aiVector2D & mid2, aiVector2D & v2, const aiVector2D & pointOfImpact) const {
    
}


void Impact2D::checkIfMassIsZero() {
    if (m1 < FP_TOLERANCE) {
        m1 = FP_TOLERANCE;
    }
    if (m2 < FP_TOLERANCE) {
        m2 = FP_TOLERANCE;
    }
}
