
#include <QMessageBox>
#include <QIntValidator>

#include "CConfigurator.h"
#include "CMovementDialog.h"
#include "CRoomManager.h"
#include "utils.h"
#include "CStacksManager.h"

CMovementDialog::CMovementDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);                        
    setWindowTitle(tr("Room's Movement Dialog"));
    lineEdit_x->setValidator(new QIntValidator(this));
    lineEdit_y->setValidator(new QIntValidator(this));
    lineEdit_z->setValidator(new QIntValidator(this));
}

void CMovementDialog::run()
{
    lineEdit_x->setText(QString("0"));
    lineEdit_y->setText(QString("0"));
    lineEdit_z->setText(QString("0"));
}

void CMovementDialog::accept()
{
    QList<int> ids;
    CRoom *r;


    printf("Accepted!\r\n");
    x = lineEdit_x->text().toInt();
    y = lineEdit_y->text().toInt();
    z = lineEdit_z->text().toInt();


    // now proceed if needed to the actuall operation
    print_debug(DEBUG_INTERFACE, "movement dialog accepted");
    printf("movement dialog accepted\r\n");
    if (x != 0 || y != 0 || z != 0) {
        print_debug(DEBUG_INTERFACE, "moving rooms by shift : x %i, y %i, z %i", x, y, z);

        printf("moving rooms by shift : x %i, y %i, z %i\r\n", x, y, z);
        
        // collect the room for the movement            
        if (Map.selections.isEmpty() == false) {
            ids = Map.selections.getList();
        } else {
            if (stacker.amount() != 1) {
                QMessageBox::critical(this, "Movement Dialog", QString("You are not in sync!"));
                done(Accepted);
                return;
            } 
            ids.append(stacker.first()->id);
        }

        for (int i = 0; i < ids.size(); ++i) {
            r = Map.getRoom( ids.at(i) );
            r->setX( r->getX() + x);
            r->setY( r->getY() + y);
            if (z != 0) 
                r->setZ( r->getZ() + z);
        }
        toggle_renderer_reaction();
    }

    done(Accepted);
}
