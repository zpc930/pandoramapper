
#include "configurator.h"
#include "CMovementDialog.h"

CMovementDialog::CMovementDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);                        
    setWindowTitle(tr("General Settings"));

}

void CMovementDialog::run()
{
    lineEdit_x->setText("0");
    lineEdit_y->setText("0");
    lineEdit_z->setText("0");
}

void CMovementDialog::accept()
{
    printf("Accepted!\r\n");
    x = lineEdit_x->text().toInt();
    y = lineEdit_y->text().toInt();
    z = lineEdit_z->text().toInt();
    done(Accepted);
}
