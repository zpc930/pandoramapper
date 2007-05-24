#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "CSquare.h"

class Frustum {
    float         m_Frustum[6][4];
    void          normalizePlane(float frustum[6][4], int side);
    int           square_frustum_checks;

    
    enum FrustumSide
    {
        RIGHT   = 0,        // The RIGHT side of the frustum
        LEFT    = 1,        // The LEFT  side of the frustum
        BOTTOM  = 2,        // The BOTTOM side of the frustum
        TOP     = 3,        // The TOP side of the frustum
        BACK    = 4,        // The BACK side of the frustum
        FRONT   = 5         // The FRONT side of the frustum
    }; 
    
    // Like above, instead of saying a number for the ABC and D of the plane, we
    // want to be more descriptive.
    enum PlaneData
    {
        A = 0,              // The X value of the plane's normal
        B = 1,              // The Y value of the plane's normal
        C = 2,              // The Z value of the plane's normal
        D = 3               // The distance the plane is from the origin
    };
    
    int curx, cury, curz;
    int current_plane_z;

public:
    void calculateFrustum(int cx, int cy, int cz);
    bool isPointInFrustum(float x, float y, float z);
    bool isSquareInFrustum(CSquare *p);
    float distance(float x, float y, float z);
};

#endif
