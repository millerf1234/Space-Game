//  ImplementationFile for the Impact2D class
//  Impact2D.cpp
//
//  Created by Forrest Miller on 3/9/18.
//

#include "Impact2D.h"


constexpr static float FP_TOLERANCE = 0.00001f;

constexpr static float PRINT_COLLISION_DEBUG = true; //= PRINT_DEBUG_MESSAGES;
constexpr static float PRINT_COLLISION_WARNING = PRINT_DEBUG_WARNING_MESSAGES;

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

void Impact2D::computeCollisionSimple(aiVector2D & v1, aiVector2D & v2, const float& vMagnitudeChangeFactor) const {
    if (m1 == m2 || true) {
        aiVector2D temp = v1 * vMagnitudeChangeFactor;
        v1 = v2 * vMagnitudeChangeFactor;
        v2 = temp;
        return;
    }
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


void Impact2D::computeCollisionExperimentalWithEndingVeloctiyOutput(aiVector2D &va1, aiVector2D &vb1, const float &massA, const float &massB, const CollisionBox &cboxA, const CollisionBox &cboxB, float coeffOfRest) const {
    aiVector2D aChange = va1;
    aiVector2D bChange(vb1);
    
    //Call the actual function
    computeCollisionExperimental(va1, vb1, massA, massB, cboxA, cboxB, coefRest);
    //Print out va1 and vb1 after the function:
    aChange = va1 - aChange; //Head - tail
    bChange = vb1 - bChange; //Head - tail
    printf("Velocity a2 = [%.5f, %.5f], change = [%.5f, %.5f]\n", va1.x, va1.y, aChange.x, aChange.y);
    printf("Velocity b2 = [%.5f, %.5f], change = [%.5f, %.5f]\n", vb1.x, vb1.y, bChange.x, bChange.y);
}



void Impact2D::computeCollisionExperimental(aiVector2D & va1, aiVector2D & vb1, const float & massA, const float & massB, const CollisionBox & cboxA, const CollisionBox & cboxB, float restitution) const {
    //first few lines for debug:
   // (A lot of this currently assumes the objects have the same mass)
    if (PRINT_COLLISION_DEBUG) {
        std::cout << "\nDEBUG::function computeCollisionExperimental was called." << std::endl;
        std::cout << "Detected values are:\n";
        printf("Mass a = %.5f   Velocity a1 = [%2.5f, %2.5f]  mag = %.5f\n", massA, va1.x, va1.y, va1.Length()); //std::cout << "Velocity a1 = [" << va1.x; << ", " << va1.y << "]\n";
        printf("Mass b = %.5f   Velocity b1 = [%2.5f, %2.5f]  mag = %.5f\n", massB, vb1.x, vb1.y, vb1.Length()); //std::cout << "Velocity a2 = [" << vb2.x; << ", " << vb2.y << "]\n";
    }
    
    
    //Do some special case checking
    //If both velocity vectors are 0, then can't do much since rotation (angular) velocity not known, so just make them
    //fly apart so they don't get stuck
    if (va1.Length() == 0 && vb1.Length() == 0.0f) {
        va1 = cboxB.getVectorFromMidpointToPoint(cboxA.getMidpoint());
        vb1 = -va1;
        return;
    }
    
    
    if (massA == massB) { //Maybe make this check more general by seeing if masses are nearly equal instead of exactly equal
        //Check to see if one object has much greater speed than the other object
        if ( (va1.Length() < (vb1 * 0.1f).Length()) || (vb1.Length() < (va1 * 0.1f).Length()) ) {
            std::cout << "One or both objects were nearly stationary. Doing basic momentum transfer collision.wd\n";
            
            aiVector2D temp = va1 * restitution;
            if (temp.Length() < 0.01f) { //Need to give the faster ship a little bounce back
                va1 = restitution * vb1;
                vb1 *= (restitution * -1.0f);
                return;
            }
            else if (vb1.Length() < 0.01f) {
                vb1 = restitution * va1;
                va1 *= (restitution * 1.0f);
            }
            else {
            va1 = vb1 * restitution;
            vb1 = temp;
            }
            return;
           
        }
    }
    
    //Check to see if the shapes are overlapping:
    bool aHitB = false;
    bool bHitA = false;
    
    if (cboxA.isOverlapping(cboxB)) {
        aHitB = true;
        printf("aHitB = %d\n", (int)aHitB);
    }
    if (cboxB.isOverlapping(cboxA)) {
        bHitA = true;
        printf("bHitA = %d\n", (int)bHitA);
    }
    
    if (!aHitB && !bHitA) {
        printf("Oops! neithor box hit...\n");
        return; //I think? This function is expecting collision to have already happened
    }
    
    
    else if (aHitB && bHitA) { //If both boxes mutually impacted eachother
        if (PRINT_COLLISION_DEBUG) {
            printf("Detected aHitB and bHitA. Doing Impact Analysis...\n");
        }
        float T = massA * va1.Length() + massB * vb1.Length(); //T is System Kinetic Energy before impact
        if (PRINT_COLLISION_DEBUG) {
            printf("T = %.5f\n", T);
        }
        
        float velDifMin = 0.9f;
        float velDifMax = 1.1f;
        //Check to make sure Va1 and Vb1 are not nearly equal by asking if (min*va1 < vb1 < max*va1) and (min*vb1 < va1 < max*vb1)
        if ( (va1.Length() * velDifMin < vb1.Length() || va1.Length() * velDifMax > vb1.Length()) && (vb1.Length() * velDifMin < va1.Length() || vb1.Length() * velDifMax > va1.Length()) ) {
            //This all assumes equal mass
            if (PRINT_COLLISION_DEBUG) {
                printf("CollisionsOfNearlySimilarVelocityDetected. Reflecting velocities over the line of impact\n");
            }
            //Figure out the angle between the two vectors
            aiVector2D normalizedVA(va1);
            normalizedVA.Normalize();
            aiVector2D normalizedVB(vb1);
            normalizedVB.Normalize();
            
            //Compute the axis that is halfway between the velocity vectors
            //See: http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/
            //float angleBetweenVelocityVecs = acosf(vOps::dot2D(normalizedVA, normalizedVB));
            //angle of V2 relative to V1= atan2(v2.y,v2.x) - atan2(v1.y,v1.x)
            float angleBetweenVelocityVecs = atan2(normalizedVB.y, normalizedVB.x) - atan2(normalizedVA.y, normalizedVA.x);
            
            //Check to see if they are going in the same direction by checking angle
            float sameDirectionAngleLimit = PI / 45.0f;
            if (-PI/sameDirectionAngleLimit < angleBetweenVelocityVecs && angleBetweenVelocityVecs < PI/sameDirectionAngleLimit){
                //Find out which box is in front of the other based off the vector directions and box midpoints
                aiVector2D aToB = cboxA.getVectorFromMidpointToPoint(cboxB.getMidpoint());
                float vectorComparison = (aToB - va1).Length(); //Length of vector from head of a's velocity to b's midpoint
                if (vectorComparison < (cboxB.getVectorFromMidpointToPoint(cboxA.getMidpoint()) - vb1).Length() ) {
                    //Then A is behind B
                    //So transfer some of A's momentum to B
                    va1 *= 0.5f;
                    vb1 *= 1.75f;
                }
                else { //B is behind A
                    vb1 *= 0.5f;
                    va1 *= 1.75f;
                }
                return;
            }
            
            //else the angle between them is large enough to do a different style of momentum transfer
            angleBetweenVelocityVecs *= 0.5f;
            aiMatrix3x3 rotation;
            rotation.a1 = cos(angleBetweenVelocityVecs);
            rotation.a2 = sin(angleBetweenVelocityVecs);
            rotation.a3 = 0.0f;
            rotation.b1 = -sin(angleBetweenVelocityVecs);
            rotation.b2 = cos(angleBetweenVelocityVecs);
            rotation.b3 = 0.0f;
            rotation.c1 = 0.0f;
            rotation.c2 = 0.0f;
            rotation.c3 = 1.0f;
            
            aiVector2D reflectionAxis;
            if (angleBetweenVelocityVecs < 0.0f) { //Then rotate from b towards A
                aiVector3D temp(normalizedVB.x, normalizedVB.y, 0.0f);
                temp = rotation * temp;
                reflectionAxis.x = temp.x;
                reflectionAxis.y = temp.y;
            }
            else if (angleBetweenVelocityVecs > 0.0f) { //Then rotate from A towards b
                aiVector3D temp(normalizedVA.x, normalizedVA.y, 0.0f);
                temp = rotation * temp;
                reflectionAxis.x = temp.x;
                reflectionAxis.y = temp.y;
            }
            else { //If the two vectors are along the same axis
                aiVector2D temp = va1;
                vb1 = va1;
                va1 = temp;
                return;
            }
            //Reflect the two velocity vectors over this axis
            //NOTE THAT ACTUALLY DOING WHAT I WAS DOING HERE IS THE SAME THING AS JUST SWAPPING VELOCITY VECTORS
            //So just swap vectors
            
            aiVector2D temp = va1;
            vb1 = va1;
            va1 = temp;
            return;
        }
        
        //Idea for future: move both boxes back by their velocity 1 step, and see which impacts first. If still tie, then do this algorithm I am going to write. If not tie, then break and handle the later cases
        //
        // Another fancy thing to do: Take the projection of each velocity vector along the line of impact and then only change that value, thus keeping components orthoganol to the impact line intact (ACTUALLY THIS IS JUST THE SAME THING AS SWAPPING VECTORS BETWEEN INSTANCES)
        //
        // See: http://www.askamathematician.com/2015/03/q-why-does-kinetic-energy-increase-as-velocity-squared/ for why this works
        //
        //   Math problem being solved in mutual collision case
        //ma*va1 + mb*vb1 = energy = ma*va2 + mb*vb2
        //
        //    T = ma*va2 + mb*vb2
        //for vb2 gives:
        //            T  -  ma*va2
        //    vb2 = -----------------
        //               mb
        //
        // Also we have the equation for coefficient of restitution as:
        //           vb2 - va2
        //     e = ---------------
        //           va1 - vb1
        //
        // We can substitute vb2 with its equation in terms of va2, and then use that to calculate vb2
        // The formula for va2 after substituting is:
        //          T - (va1 - vb1) * e * mb
        // va2 = -------------------------------
        //                  ma + mb
        //
        // Which is exactly what this following code will do:
        //T was computed above, so just need to compute va2 and vb2
        
        //compute va2
        //Should check to make sure (va1 - vb1) isn't close to 0
        //float va2 =
        float va2 = (T - (va1.Length() - vb1.Length()*restitution*massB)) / (massA + massB);
        //then solvve for vb2
        //
        // vb2 = e * (va1 - vb1) + va2
        float vb2 = restitution * (va1.Length() - vb1.Length()) + va2;
        va1.Normalize();
        va1 *= va2;
        vb1.Normalize();
        vb1 *= vb2;
        if (PRINT_COLLISION_DEBUG) {
            std::cout << "Collision of different length velocity vectors detected. Doing momentum transfer\n";
        }
    }
    
    else if (aHitB) {
        if (PRINT_COLLISION_DEBUG) {
            printf("Detected aHitB. Doing Impact Analysis...\n");
        }
        if (isnan(va1.Length() || va1.Length() == 0)) {
            va1 = aiVector2D(1.0f, 1.0f);
            //return;
        }
        if (isnan(vb1.Length() || vb1.Length() == 0)) {
            vb1 = aiVector2D(-1.0f, -1.0f);
            return;
        }
        
        vb1 = ((va1 * 3.0f) + (vb1 * 0.85f));
        va1 = va1 * -1.0f * restitution * 0.5f;
        
    }
    
    else { //bHitA
        if (PRINT_COLLISION_DEBUG) {
            printf("Detected bHitA. Doing Impact Analysis...\n");
        }
        if (isnan(va1.Length() || va1.Length() == 0)) {
            va1 = aiVector2D(1.0f, 1.0f);
            //return;
        }
        if (isnan(vb1.Length() || vb1.Length() == 0)) {
            vb1 = aiVector2D(-1.0f, -1.0f);
            return;
        }

        va1 = vb1 + va1;
        vb1 = vb1 * -1.0f * restitution * 0.5f;
    }
    
}
