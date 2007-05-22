

#include <QSet>

#include "CSelectionManager.h"

void CSelectionManager::select(unsigned int id)
{
    if (isSelected(id) == false) 
        selection.insert(id);
}

void CSelectionManager::unselect(unsigned int id)
{
    selection.remove(id);
}


void CSelectionManager::resetSelection()
{
    selection.clear();
}

CSelectionManager::CSelectionManager() 
{
    selection.clear();
}

CSelectionManager::~CSelectionManager()
{
    selection.clear();
}
