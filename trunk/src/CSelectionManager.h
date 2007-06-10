#ifndef CSELECTIONMANAGER_H
#define CSELECTIONMANAGER_H

#include <QObject>
#include <QSet>
#include <QPoint>

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

    unsigned int getFirst() { return selection.values().at(0); }
    unsigned int get(int index) { return selection.values().at(index); }

    int size() {return selection.size(); }
    void exclusiveSelection(unsigned int id); 

    QList<int>  getList() { return selection.toList(); }
    bool isEmpty() { return selection.empty(); }

signals:
  void roomSelected(unsigned int id);
};

class CMouseState : public QObject {
Q_OBJECT

public:
    bool          LeftButtonPressed;
    bool          RightButtonPressed;
    QPoint        oldPos;       // last registred position
    QPoint        origPos;      // original position of click


    CMouseState();
    int delta(QPoint pos);  // delta from original Click position to given pos
}; 

#endif
