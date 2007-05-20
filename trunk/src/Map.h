#ifndef ROOMS_H 
#define ROOMS_H 

#include <vector>
#include "defines.h"
#include "CRoom.h"
#include "Regions.h"
using namespace std;

class CPlane;
class CSquare;


#define MAX_ROOMS       30000		/* maximal amount of rooms */

class roommanager {

    QList<CRegion *>    regions;
public:
  vector<CRoom *> rooms;   		/* rooms */
  CRoom *ids[MAX_ROOMS];	/* array of pointers */

  
  unsigned int size()  { return rooms.size(); }
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  /* plane support */  
  CPlane        *planes;        /* planes/levels of the map, sorted by the Z coordinate, lower at first */
  void          addToPlane(CRoom *room);
  void          removeFromPlane(CRoom *room);
  void          expandPlane(CPlane *plane, CRoom *room);

  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addRoomNonsorted(CRoom *room);   /* use only at loading */
  void addRoom(CRoom *room);
  
  
  CRoom * getRoom(unsigned int id); /* get room by id */
  QByteArray getName(unsigned int id);
  
  
    
  int tryMergeRooms(CRoom *room, CRoom *copy, int j);
    


  void fixFreeRooms();
  CRegion *getRegionByName(QByteArray name);
  bool addRegion(QByteArray name);
  void addRegion(CRegion *reg);
  
  void sendRegionsList();  
  
  QList<CRegion *> getAllRegions();
  
  
  void deleteRoom(CRoom *r, int mode);  /* user interface function */
  void smallDeleteRoom(CRoom *r);  /* user interface function */
};

extern class roommanager Map;/* room manager */

#endif
