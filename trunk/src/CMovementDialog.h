#ifndef CMOVEMENTDIALOG_H 
#define CMOVEMENTDIALOG_H

#include <QDialog>
#include "ui_movementdialog.h"


class CMovementDialog : public QDialog, public Ui::MovementDialog {
Q_OBJECT
public:
    CMovementDialog(QWidget *parent = 0);
    int x;
    int y;
    int z;
    
    void run();
public slots:
    virtual void accept();
};


#endif
