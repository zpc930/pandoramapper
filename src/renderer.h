#ifndef RENDERER_H 
#define RENDERER_H 


#include "defines.h"
#include "CRoom.h"
#include "Map.h"
#include "configurator.h"
#include "Frustum.h"

class QFont;

//#define DIST_Z	2	/* the distance between 2 rooms */
#define BASE_Z  -12	/* the distance to the "camera" */
#define ROOM_SIZE 0.45f	/* the size of the rooms walls */

#define MAXHITS 200

class RendererWidget : public QGLWidget
{
    Q_OBJECT
    
    GLfloat       colour[4];
    GLuint        global_list;
    int           curx;
    int           cury;
    int           curz;			/* current rooms position */ 
    Frustum       frustum;
    QFont         textFont;
    
    int           lowerZ;
    int           upperZ;

    unsigned int last_drawn_marker;
    unsigned int last_drawn_trail;
    
    GLuint selectBuf[MAXHITS];
    
    void glDrawMarkers();
    void drawMarker(int, int, int, int);
    void glDrawRoom(CRoom *p);
    void glDrawCSquare(CSquare *p, int renderingMode);
    
    void setupViewingModel(  int width, int height  );
    void renderPickupObjects();  
    void renderPickupRoom(CRoom *p);


    // this sets curx, cury, curz based on hrm internal rules
    void setupNewBaseCoordinates();
public:
    GLfloat       angley;
    GLfloat       anglex;
    GLfloat       anglez;
    float         userx;
    float         usery;
    float         userz;		/* additional shift added by user */
    int           userLayerShift;       // this affect curz.
    
    int           current_plane_z;        
    GLuint        basic_gllist;
    
        
    int           rooms_drawn_csquare;
    int           rooms_drawn_total;
    
    RendererWidget( QWidget *parent = 0);
    
    
    void display(void);
    
    
    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();
    void changeUserLayerShift(int byValue) { userLayerShift += byValue; curz += byValue; }
    
    bool doSelect(QPoint pos, unsigned int &id);
    
public slots:
    void draw();
};



#endif
