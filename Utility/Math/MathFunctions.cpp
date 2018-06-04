//
//  MathFunctions.cpp
//
//  Created by Forrest Miller on 5/31/18.
//

#include "MathFunctions.h"

namespace MathFunc {
    
    float getRandomInRange(float min, float max) {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937 mt_rand((unsigned int)seed);
        auto real_rand = std::bind(std::uniform_real_distribution<float>(min, max), mt_rand);
        return real_rand();
    }
    
    int getRandomInRange(int min, int max) {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937 mt_rand((unsigned int)seed);
        auto int_rand = std::bind(std::uniform_real_distribution<int>(min, max), mt_rand);
        return int_rand();
    }
    
    
    
}
