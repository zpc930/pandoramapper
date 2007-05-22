#ifndef CSELECTIONMANAGER_H
#define CSELECTIONMANAGER_H

#include <QObject>
#include <QSet>

class CSelectionManager : public QObject {
Q_OBJECT
    QSet<int> selection;
    
public:
    CSelectionManager();
    ~CSelectionManager();
    void resetSelection();

    inline bool isSelected(unsigned int id) { return selection.contains(id); }

    void select(unsigned int id);
    void unselect(unsigned int id);

    bool isEmpty() { return selection.empty(); }
    
};

#endif
