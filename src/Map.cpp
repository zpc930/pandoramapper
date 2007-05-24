/* Implementation of room manager of Pandora Project (c) Azazello 2003 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <QDateTime>
#include <vector>
//using namespace std;

#include "defines.h"
#include "configurator.h"

#include "Map.h"
#include "utils.h"
#include "tree.h"
#include "stacks.h"
#include "dispatch.h"
#include "renderer.h"

class roommanager Map;



/*------------ merge_rooms ------------------------- */
int roommanager::tryMergeRooms(CRoom *r, CRoom *copy, int j)
{
  int i;
  CRoom *p;

  if (j == -1) {
    /* oneway ?! */
    print_debug(DEBUG_ROOMS, "fixing one way in previous room, repointing at merged room");
    
    p = getRoom(oneway_room_id);
    for (i = 0; i <= 5; i++)
        if (p->isExitLeadingTo(i, copy) == true) 
            p->setExit(i, r);
    
    smallDeleteRoom(copy);


    stacker.put(r);
    return 1;
  }
  if ( r->isExitUndefined(j) ) {
    r->setExit(j, copy->exits[j] );
                      
    p = copy->exits[j] ;
    if (p->isExitLeadingTo( reversenum(j), copy) == true)
        p->setExit( reversenum(j), r);
        
    smallDeleteRoom(copy);

    stacker.put(r);
    return 1;
  }
  return 0;
}

/* ------------ fixfree ------------- */
void roommanager::fixFreeRooms()
{
    unsigned int i;

    for (i = 1; i < MAX_ROOMS; i++)
	if (ids[i] == NULL) {
	    next_free = i;
	    return;
	}

    printf
	("roomer: error - no more space for rooms in ids[] array! reached limit\n");
    exit(1);
}

/* ------------ fixfree ENDS -------- */


/* ------------ getroom ------------- */
CRoom *roommanager::getRoom(unsigned int id)
{
  return ids[id];
}

/* ----------- getroom ENDS --------- */

QByteArray roommanager::getName(unsigned int id) 
{     
    if (ids[id])
        return (*(ids[id])).getName();
    return "";
}

/* ------------ addroom --------------*/
void roommanager::addRoomNonsorted(CRoom *room)
{
  if (ids[room->id] != NULL) {
      printf
          ("Error while adding new element to database! This id already exists!\n");
      exit(1);
  }


  rooms.push_back(room);
  ids[room->id] = room;	/* add to the first array */
  NameMap.addName(room->getName(), room->id);	/* update name-searhing engine */



  fixFreeRooms();
  addToPlane(room);
}

void roommanager::addRoom(CRoom *room)
{
  addRoomNonsorted(room);
}
/* ------------ addroom ENDS ---------- */

/*------------- Constructor of the room manager ---------------*/
roommanager::roommanager()
{
    init();
}


void roommanager::init()
{
    printf("Roommanager INIT.\r\n");

    next_free = 1;

    print_debug(DEBUG_ROOMS, "In roomer.init()");
  
    /* adding first (empty) root elements to the lists */
    rooms.clear();
    regions.clear();
    
    
    
    CRegion *region = new CRegion;
    region->setName("default");
    
    regions.push_back(region);
    
    
    ids[0] = NULL;
    planes = NULL;
}

/*------------- Constructor of the room manager ENDS  ---------------*/

CRegion *roommanager::getRegionByName(QByteArray name)
{
    CRegion    *region;
    for (int i=0; i < regions.size(); i++) {
        region = regions[i];
        if (region->getName() == name) 
            return region;
    }
    return NULL;
}

bool roommanager::addRegion(QByteArray name)
{
    CRegion    *region;
    
    if (getRegionByName(name) == false) {
        region = new CRegion();
        region->setName( name );
        regions.push_back(region);
        return true;
    } else {
        return false;
    }
    
}

void roommanager::addRegion(CRegion *reg)
{
    if (reg != NULL) 
        regions.push_back(reg);
}


void roommanager::sendRegionsList()
{
    CRegion    *region;
    send_to_user( "Present regions: \r\n");
    for (int i=0; i < regions.size(); i++) {
        region = regions[i];
        send_to_user("  %s\r\n", (const char *) region->getName() );
    }
    

}

QList<CRegion *> roommanager::getAllRegions()
{
    return regions;
}


/* -------------- reinit ---------------*/
void roommanager::reinit()
{
    next_free = 1;
    {
        CPlane *p, *next;
        
        printf("Resetting Cplane structures ... \r\n");
        p = planes;
        while (p) {
            next = p->next;
            delete p;
            p = next;
        }
        planes = NULL;
    }

    memset(ids, 0, MAX_ROOMS * sizeof (CRoom *) );        
/*
    for (i = 0; i < MAX_ROOMS; i++)
      ids[i] = NULL;
    ids[0] = rooms;
*/
    
    rooms.clear();     
    NameMap.reinit();
}

/* -------------- reinit ENDS --------- */

/* ------------ delete_room --------- */
/* mode 0 - remove all links in other rooms together with exits and doors */
/* mode 1 - keeps the doors and exits in other rooms, but mark them as undefined */
void roommanager::deleteRoom(CRoom *r, int mode)
{
    int k;
    unsigned int i;
    
    
    if (r->id == 1) {
	printf("Cant delete base room!\n");
	return;
    }

    /* have to do this because of possible oneways leading in */
    for (i = 0; i < rooms.size(); i++) 
	for (k = 0; k <= 5; k++)
	    if (rooms[i]->isExitLeadingTo(k, r) == true) {
                if (mode == 0) {
                    rooms[i]->removeExit(k);
                } else if (mode == 1) {
                    rooms[i]->setExitUndefined(k);
                }
	    }

    smallDeleteRoom(r);
}

/* --------- _delete_room ENDS --------- */

/* ------------ small_delete_room --------- */
void roommanager::smallDeleteRoom(CRoom *r)
{
    if (r->id == 1) {
	printf("ERROR (!!): Attempted to delete the base room!\n");
	return;
    }
    removeFromPlane(r);
    stacker.removeRoom(r->id);
    
    vector<CRoom *>::iterator i;
    ids[ r->id ] = NULL;
    
    for (i = rooms.begin(); i != rooms.end(); i++)
        if ((*i)->id == r->id ) {
            printf("Deleting the room from rooms vector.\r\n");
            i = rooms.erase(i);
            break;
        }
    
    delete r;    
    fixFreeRooms();
}
/* --------- small_delete_room ENDS --------- */

void roommanager::removeFromPlane(CRoom *room)
{
    CPlane *p;
    
    p = planes;
    while (p->z != room->getZ()) {
        if (!p) {
            printf(" FATAL ERROR. remove_fromplane() the given has impossible Z coordinate!\r\n");
            return;     /* no idea what happens next ... */
        }
        p = p->next;
    }

    p->squares->remove(room);
}

void roommanager::expandPlane(CPlane *plane, CRoom *room)
{
    CSquare *p, *new_root = NULL;
    int size;
    
    p = plane->squares;
    
/*    printf("Preparing to expand the plane lx ly: %i %i, rx ry: %i %i, cx cy: %i %i, for room x y: %i %i\r\n",
            p->leftx, p->lefty, p->rightx, p->righty, p->centerx, p->centery, room->x, room->y);
*/    
    while ( p->isInside(room) != true ) {
        /* plane fork/expanding cycle */
        
        size = p->rightx - p->leftx;
        
        switch ( p->getMode(room) )
        {
            case  CSquare::Left_Upper:
                new_root = new CSquare(p->leftx - size, p->lefty + size, p->rightx, p->righty);
                new_root->subsquares[ CSquare::Right_Lower ] = p;
                break;
            case  CSquare::Right_Upper:
                new_root = new CSquare(p->leftx,  p->lefty + size, p->rightx + size, p->righty);
                new_root->subsquares[ CSquare::Left_Lower ] = p;
                break;
            case  CSquare::Right_Lower:
                new_root = new CSquare(p->leftx,  p->lefty, p->rightx + size, p->righty - size);
                new_root->subsquares[ CSquare::Left_Upper ] = p;
                break;
            case  CSquare::Left_Lower:
                new_root = new CSquare(p->leftx - size,  p->lefty, p->rightx , p->righty - size);
                new_root->subsquares[ CSquare::Right_Upper ] = p;
                break;
        }
        
        p = new_root;
    }    

/*    printf("Ok, it fits. Adding!\r\n");
*/    
    p->add(room);
    plane->squares = p;
}


void  roommanager::addToPlane(CRoom *room)
{
    CPlane *p, *prev, *tmp;

    
    
    if (planes == NULL) {
        planes = new CPlane(room);
        return;
    }
    
    p = planes;
    prev = NULL;
    while (p) {
        if (room->getZ() < p->z) {
            tmp = new CPlane(room);
            tmp->next = p;
            if (prev)
                prev->next = tmp;
            else 
                planes = tmp;
            return;
        }
        /* existing plane with already set borders */
        if (room->getZ() == p->z) {
            expandPlane(p, room);
            return;
        }
        prev = p;
        p = p->next;
    }
    
    /* else .. this is a plane with highest yet found Z coordinate */
    /* we add it to the end of the list */
    prev->next = new CPlane(room);
}
