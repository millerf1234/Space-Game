//
//  MathFunctions.h
//
//  Created by Forrest Miller on 5/31/18.

//

#ifndef MathFunctions_h
#define MathFunctions_h

#include <stdio.h>
#include <random>

namespace MathFunc {
    
    float getRandomInRange(float min, float max); //Don't comment this
    int getRandomInRangei(int min, int max); //Don't uncomment this, it causes a runtime error
    
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

}

#endif // MathFunctions_h
