//
//  VectorOps.h
//
//  Created by Forrest Miller on 3/9/18.
//

#ifndef VectorOps_h
#define VectorOps_h

#include <stdio.h>
#include <assimp/Importer.hpp> //Gives access to the aiVector3D
#include <cmath>

namespace vOps {
float static getTriangleArea(const aiVector2D& p0, const aiVector2D& p1, const aiVector2D& p2) {
    return abs(((-p1.x*p0.y + p2.x*p0.y + p0.x*p1.y - p2.x*p1.y - p0.x*p2.y + p1.x*p2.y) / 2.0f));
    //(-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2); //Computation to be performed (basically just a determinant)
}

float static getTriangleArea(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2) {
    return abs((-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2) / 2.0f);
}

float static getParallelogramArea(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2) {
    return abs(-x1*y0 + x2*y0 + x0*y1 - x2*y1 - x0*y2 + x1*y2);
}

//Here are some math functions that are used:
//Need a dot product operation
float static dot(const aiVector3D & v1, const aiVector3D & v2) {
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

float static dot2D(const aiVector2D & v1, const aiVector2D & v2) {
    return (v1.x*v2.x + v1.y*v2.y);
}

//float cross(const aiVector3D & v1, const aiVector3D & v2);

//Need a function to get the vector between two points
aiVector2D static getVectorBetween(const aiVector2D& head, const aiVector2D& tail) {
    return aiVector2D(head.x-tail.x, head.y-tail.y);
}

float static getDistance(const aiVector2D & p1, const aiVector2D & p2) {
    return (getVectorBetween(p1, p2).Length());
}

aiVector2D static getMidpoint(const aiVector2D & p1, const aiVector2D & p2) {
    aiVector2D midpointDisplacement = getVectorBetween(p2, p1); //p2 head, p1 tail
    midpointDisplacement *= 0.5f; //Shorten the vector by half
    return (p1 + midpointDisplacement);
}


//For some reason I have prototypes for the next 3 functions...
//Get the largest absolute value in an array
float static getMaxFromArray(float * data, int dataSize);
//Gets the most positive value in an array of floats
float static getMaxFromArrayPositiveOnly(float * data, int dataSize);
//Gets the most negative value in an array of floats
float static getMaxFromArrayNegativeOnly(float * data, int dataSize);


float static getMaxFromArray(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (abs(data[i]) > max) {
            max = abs(data[i]);
        }
    }
    return max;
}

float static getMaxFromArrayPositiveOnly(float * data, int dataSize) {
    float max = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        //if (data[i] < 0.0f) {continue;} //
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}

//Finds the most negative value in an array of floats
float static getMaxFromArrayNegativeOnly(float * data, int dataSize) {
    float negMax = 0.0f;
    for (int i = 0; i < dataSize; ++i) {
        if (data[i] < negMax) {
            negMax = data[i];
        }
    }
    return negMax;
}
};

#endif /* VectorOps_h */
