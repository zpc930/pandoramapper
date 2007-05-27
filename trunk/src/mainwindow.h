#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>


#include "RoomEditDialog.h"
#include "ConfigWidget.h"
#include "SpellsDialog.h"
#include "CMovementDialog.h"
#include "CLogDialog.h"
#include "renderer.h"

#include "CActionManager.h"

void toggle_renderer_reaction();
void notify_analyzer();

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    CActionManager *actionManager;
    CMouseState     mouseState;
    
    QLabel        *locationLabel; 
    QLabel        *formulaLabel; 
    QLabel        *modLabel; 

    QMenu       *fileMenu;
    QMenu       *mappingMenu;
    QMenu       *actionsMenu;
    QMenu       *optionsMenu;
    QMenu       *logMenu;
    QMenu       *helpMenu;

 
    QPoint mousePosInRenderer( QPoint pos );
    bool checkMouseSelection( QMouseEvent *e ); 



public:
    MainWindow(QWidget *parent = 0);

    RendererWidget *renderer;
    ConfigWidget      *generalSettingsDialog;
    RoomEditDialog    *edit_dialog;
    SpellsDialog      *spells_dialog;
    CMovementDialog   *movementDialog;
    CLogDialog        *logdialog;

    QAction       *hide_status_action;
    QAction       *hide_menu_action;


    void update_status_bar();
    void editRoomDialog(unsigned int id);


private slots:
  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *e);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);
  void keyPressEvent( QKeyEvent * );

public slots:
  void hide_menu();
  void hide_status();
  void hide();                              /* hide all extra widgets */

  void moveRoomDialog();

signals:
  void newLocationLabel(const QString &label);
  void newModLabel(const QString &label);


};


extern class MainWindow *renderer_window;



#endif
