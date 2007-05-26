

#include <QSet>


#include "CSelectionManager.h"
#include "utils.h"
#include "Map.h"
#include "mainwindow.h"


void CSelectionManager::select(unsigned int id)
{
    if (isSelected(id) == false) {
        selection.insert(id);
        print_debug(DEBUG_INTERFACE, "selecting %i", id);
    }
    toggle_renderer_reaction();
}

void CSelectionManager::unselect(unsigned int id)
{
    selection.remove(id);
    toggle_renderer_reaction();
}


void CSelectionManager::resetSelection()
{
    selection.clear();
    toggle_renderer_reaction();
}

CSelectionManager::CSelectionManager() 
{
    selection.clear();
}

CSelectionManager::~CSelectionManager()
{
    selection.clear();
}


void CSelectionManager::exclusiveSelection(unsigned int id) 
{
    if (selection.contains(id)) 
        return;
    else {
        resetSelection();
        select(id);
    }   
}


//########################### CMouseState ########################

CMouseState::CMouseState() 
{
    LeftButtonPressed = false;
    RightButtonPressed = false;

    oldPos.setX(0);
    oldPos.setY(0);

    origPos.setX(0);
    origPos.setY(0);
}


int CMouseState::delta(QPoint pos)
{
    int x = origPos.x()-pos.x();
    int y = origPos.y()-pos.y();

    return x*x + y*y;
}
