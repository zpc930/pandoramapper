#ifndef DEFINES_H 
#define DEFINES_H 

#include <Qt>

#define MAX_ROOMS       30000		/* maximal amount of rooms */


#define MAX_STR_LEN     400
#define MAX_DATA_LEN 3072
#define MAX_LINES_DESC  20

#define NORTH           0         
#define EAST            1
#define SOUTH           2         
#define WEST            3
#define UP              4
#define DOWN            5

/* coordinate's cap */
#define MAX_X           32000
#define MIN_X           -32000


/* global flags */
extern int glredraw;		/* redraw is needed */
extern QString *logFileName;

void toggle_renderer_reaction();
void notify_analyzer();


#endif

