//  Use this for a sperical shield effect around each player?
//  CollisionSphere.hpp
//
//  Created by Forrest Miller on 2/28/18.
//

#ifndef CollisionSphere_h
#define CollisionSphere_h

#include <iostream>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D

#include "Quaternion.h"

class CollisionSphere {
private:
    aiVector3D center;
    float radius;
    
public:
    CollisionSphere();
    CollisionSphere(const aiVector3D& center, float radius);
    
    void changeRadius(float);
    float getRadius() const;
    
    void changeCenter(const aiVector3D newCenter);
    void translate(float x, float y, float z);
    
};


#endif /* CollisionSphere_h */
