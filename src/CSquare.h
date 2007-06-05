#ifndef CSQUARE_H
#define CSQUARE_H

#include <QVector>

class CRoom;


class CSquare {
public:

    enum SquareTypes {
        Left_Upper = 0,             
        Right_Upper = 1,            
        Left_Lower = 2,
        Right_Lower = 3
    };

    /* subsquares */
    CSquare     *subsquares[4];
    /* coordinates of this square's left (upper) and right (lower) points */
    int         leftx, lefty;
    int         rightx, righty;
    int         centerx, centery;
    
    /* amount of rooms in this square, -1 for empty */
    QVector<CRoom *> rooms;

    CSquare(int leftx, int lefty, int rightx, int righty);
    CSquare();
    ~CSquare();

    /* mode == SquareType */
    int         getMode(CRoom *room);
    int         getMode(int x, int y);
    bool        toBePassed();
    bool        isInside(CRoom *room);  
    
    void        addSubsquareByMode(int mode);
    void        addRoomByMode(CRoom *room, int mode);
        
    void        add(CRoom *room);
    void        remove(CRoom *room);
};

class CPlane {
    /* levels/planes. each plane stores a tree of CSquare type and its z coordinate */
public:
    int         z;

    CSquare     *squares;
    CPlane      *next;

    CPlane();
    ~CPlane();
    CPlane(CRoom *room);
};



#endif
