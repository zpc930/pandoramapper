#ifndef CLOGDIALOG_H 
#define CLOGDIALOG_H

#include <QDialog>
#include "ui_logdialog.h"


class CLogDialog : public QDialog, public Ui::LogDialog {
Q_OBJECT
public:
    CLogDialog(QWidget *parent = 0);
    
    void run();
public slots:
    virtual void accept();
};


#endif
