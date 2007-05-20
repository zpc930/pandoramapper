#ifndef RENDERER_H 
#define RENDERER_H 


#include "defines.h"
#include "CRoom.h"
#include "Map.h"
#include "configurator.h"
#include "Frustum.h"



class RendererWidget : public QGLWidget
{
  Q_OBJECT


public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  float         userx;
  float         usery;
  float         userz;		/* additional shift added by user */

  int           current_plane_z;        /* to avoid arguments usage */
  GLuint        basic_gllist;

    
  int           rooms_drawn_csquare;
  int           rooms_drawn_total;

  RendererWidget( QWidget *parent = 0);


  void display(void);
  


protected:
  void initializeGL();
  void resizeGL( int width, int height );
  void paintGL();

private:
  GLfloat       colour[4];
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 
  Frustum       frustum;

  unsigned int last_drawn_marker;
  unsigned int last_drawn_trail;
  

  void glDrawMarkers();
  void drawMarker(int, int, int, int);
  void glDrawRoom(CRoom *p);
  
  void glDrawCSquare(CSquare *p);

public slots:
  void draw();
};



#endif
