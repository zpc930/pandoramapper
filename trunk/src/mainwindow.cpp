#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "utils.h"
#include "stacks.h"
#include "engine.h"
#include "userfunc.h"
#include "forwarder.h"
#include "CActionManager.h"

/* global classless */

void toggle_renderer_reaction()
{
    print_debug(DEBUG_INTERFACE,"Toggling renderer reaction\r\n");
    proxy->startRendererCall();
//    print_debug(DEBUG_RENDERER, "toggle_renderer_reaction called()");
//    QKeyEvent *k = new QKeyEvent(QEvent::KeyPress, Qt::Key_R,0, "r", false , 1);
//    QApplication::postEvent( renderer_window->renderer, k );
}

void notify_analyzer()
{
    proxy->startEngineCall();
//    QKeyEvent *k = new QKeyEvent(QEvent::KeyPress, Qt::Key_C,0, "c", false , 1);
//    QApplication::postEvent( renderer_window->renderer, k );
}


/*  globals end */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow( parent)
{
    spells_dialog = NULL;
    edit_dialog = NULL;
    generalSettingsDialog = NULL;
    

    userland_parser = new Userland();
    actionManager = new CActionManager(this);
    
    print_debug(DEBUG_INTERFACE, "in mainwindow constructor");
    
    setWindowTitle("Pandora");
    renderer =  new RendererWidget( this );
    setCentralWidget( renderer );
    
    setGeometry( conf->get_window_rect() );
    
    connect(proxy, SIGNAL(connectionEstablished()), this, SLOT(enable_online_actions()), Qt::AutoConnection );
    connect(proxy, SIGNAL(connectionLost()), this, SLOT(disable_online_actions()), Qt::AutoConnection );



    /* now building a menu and adding actions to menu */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(actionManager->newAct);
    fileMenu->addAction(actionManager->publishAct);
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->openAct);  
    fileMenu->addAction(actionManager->reloadAct);  
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->saveAct);  
    fileMenu->addAction(actionManager->saveAsAct);  
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->quitAct);  


    actionsMenu = menuBar()->addMenu(tr("&Room"));
    actionsMenu->addAction(actionManager->roomeditAct);
    actionsMenu->addAction(actionManager->deleteAct);
    actionsMenu->addAction(actionManager->deleteFullyAct);
    actionsMenu->addAction(actionManager->mergeAct);
    
    mappingMenu = menuBar()->addMenu(tr("&Mapping"));
    mappingMenu->addAction(actionManager->mappingAct);
    mappingMenu->addAction(actionManager->automergeAct);
    mappingMenu->addAction(actionManager->angryLinkerAct);

    hide_status_action = new QAction(tr("Hide Status Bar"), this);
    hide_status_action->setShortcut(tr("F11"));
    hide_status_action->setStatusTip(tr("Hides the Statusbar"));
    hide_status_action->setCheckable(true);
    hide_status_action->setChecked(false);
    connect(hide_status_action, SIGNAL(triggered()), this, SLOT(hide_status()));    
    
    hide_menu_action= new QAction(tr("Hide Menu Bar"), this);
    hide_menu_action->setShortcut(tr("F12"));
    hide_menu_action->setStatusTip(tr("Hides the Menubar"));
    hide_menu_action->setCheckable(true);
    hide_menu_action->setChecked(false);
    connect(hide_menu_action, SIGNAL(triggered()), this, SLOT(hide_menu()));    


    optionsMenu = menuBar()->addMenu(tr("&Configuration"));
    optionsMenu->addAction(hide_status_action);
    optionsMenu->addAction(hide_menu_action);
    optionsMenu->addAction(actionManager->always_on_top_action);  
    optionsMenu->addAction(actionManager->emulationAct);  
    optionsMenu->addSeparator();
    optionsMenu->addAction(actionManager->setupGeneralAct);  
    optionsMenu->addAction(actionManager->spellsAct);  
    optionsMenu->addSeparator();
    optionsMenu->addAction(actionManager->saveConfigAct);
    //  optionsMenu->addAction(saveConfigAsAct);
    optionsMenu->addAction(actionManager->loadConfigAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(actionManager->aboutAct);
    helpMenu->addAction(actionManager->aboutQtAct);
    
    
    /* status bar magicz */
    locationLabel = new QLabel("NO_SYNC"); 
    locationLabel->setAlignment(Qt::AlignHCenter); 
    locationLabel->setMinimumSize(locationLabel->sizeHint()); 
    
    formulaLabel = new QLabel(); 
    
    modLabel = new QLabel(tr("     ")); 
    modLabel->setAlignment(Qt::AlignHCenter); 
    modLabel->setMinimumSize(modLabel->sizeHint());      
    modLabel->clear(); 
    
    statusBar()->addWidget(locationLabel); 
    statusBar()->addWidget(formulaLabel, 1); 
    statusBar()->addWidget(modLabel); 
    
    connect(this, SIGNAL(newLocationLabel(const QString &)),
            locationLabel, SLOT(setText(const QString &)));
    connect(this, SIGNAL(newModLabel(const QString &)),
            modLabel, SLOT(setText(const QString &)));
    
    actionManager->disable_online_actions();
}


void MainWindow::hide_status()
{
  print_debug(DEBUG_INTERFACE, "hide/show status called");

  if (hide_status_action->isChecked() ) 
  {
    statusBar()->hide();  
  } else {
    statusBar()->show();
  }
}

void MainWindow::editRoomDialog(unsigned int id)
{
    if (!edit_dialog) {
        edit_dialog = new RoomEditDialog(this);
    }

    edit_dialog->clear_data();
    edit_dialog->load_room_data(id);
        
    edit_dialog->show();
    edit_dialog->raise();
    edit_dialog->activateWindow();
}


void MainWindow::hide()
{
  print_debug(DEBUG_INTERFACE, "hide/show all function");
  
  menuBar()->hide();
  statusBar()->hide();

  
  hide_menu_action->setChecked(true);
  hide_status_action->setChecked(true);
}

void MainWindow::hide_menu()
{
  if (hide_menu_action->isChecked()) 
  {
    menuBar()->hide();  
  } else {
    menuBar()->show();
  }
}


void MainWindow::update_status_bar()
{
  char str[20];
  
  print_debug(DEBUG_INTERFACE, "Updating status bar\r\n");

  if (conf->get_data_mod() )
    emit newModLabel("Data: MOD ");     
  else 
    emit newModLabel("Data: --- ");
  

  stacker.getCurrent(str);
  emit newLocationLabel(str);
  print_debug(DEBUG_INTERFACE, "Done updating interface!\r\n");
}

void MainWindow::keyPressEvent( QKeyEvent *k )
{

    print_debug(DEBUG_INTERFACE, "Processing events in keyPressEvent\r\n");
    switch ( k->key() ) {
         
        case Qt::Key_X :
            renderer->userz += 1;
            glredraw = 1;
            break;
    
         case Qt::Key_Y:
            renderer->userz -= 1;
            glredraw = 1;
            break;
    
         case Qt::Key_Q:
            renderer->userx -= 1;
            glredraw = 1;
            break;
         
         case Qt::Key_W:
            renderer->userx += 1;
            glredraw = 1;
            break;
         
         case Qt::Key_A:
            renderer->usery += 1;
            glredraw = 1;
            break;
    
         case Qt::Key_S:
            renderer->usery -= 1;
            glredraw = 1;
            break;
    
        case Qt::Key_Up:
          renderer->anglex += 5;
          glredraw = 1;
          break;
        case Qt::Key_Down:
          renderer->anglex -= 5;
          glredraw = 1;
          break;
        case Qt::Key_Left:
          renderer->angley -= 5;
          glredraw = 1;
          break;
        case Qt::Key_Right:
          renderer->angley += 5;
          glredraw = 1;
          break;
        case Qt::Key_PageUp:
          renderer->anglez += 5;
          glredraw = 1;
          break;
        case Qt::Key_PageDown:
          renderer->anglez -= 5;
          glredraw = 1;
          break;
        
         case Qt::Key_Escape:           
            renderer->angley = 0;
            renderer->anglex = 0;
            renderer->anglez = 0;
            renderer->userx = 0;
            renderer->usery = 0;
            renderer->userz = BASE_Z;		
            glredraw = 1;
            break;				

         case Qt::Key_F12:
	    hide_menu_action->setChecked(!hide_menu_action->isChecked());
            hide_menu();
            break;
         case Qt::Key_F11:
	    hide_status_action->setChecked(!hide_status_action->isChecked());
            hide_status();
            break;				
         case Qt::Key_F10:
            //hide_roominfo();
            break;				
    }
    update_status_bar();
    renderer->display();
    print_debug(DEBUG_INTERFACE, "Done processing events at keyEventPress\r\n");
}

void MainWindow::mousePressEvent( QMouseEvent *e)
{
  
    if (e->button() == Qt::LeftButton) {
        mouseState.LeftButtonPressed = true;


    } else {
        mouseState.RightButtonPressed = true;
    }
    mouseState.oldPos = e->pos();
    mouseState.origPos = e->pos();
  
  
}

QPoint MainWindow::mousePosInRenderer( QPoint pos ) {

    QPoint inFramePos = pos;
    
    if (menuBar()->isHidden() == false) {
        int height = menuBar()->height();
        inFramePos.setY( inFramePos.y() -  height);
    }

    return inFramePos;
}

bool MainWindow::checkMouseSelection( QMouseEvent *e ) 
{
    unsigned int id;

    if (mouseState.delta( e->pos() ) <= 100) {
        if (renderer->doSelect( mousePosInRenderer( e->pos() ), id ) == true) {
            if (e->modifiers().testFlag( Qt::ControlModifier ) == true ) {
                if (Map.selections.isSelected( id ) == true) 
                    printf("The room already WAS selected!\r\n");

                if (Map.selections.isSelected( id) == true)
                    Map.selections.unselect( id );
                else 
                    Map.selections.select( id );
            } else 
                Map.selections.exclusiveSelection( id );
            
            return true;
        }
    } 

    return false;
}

void MainWindow::mouseReleaseEvent( QMouseEvent *e)
{


    if (e->button() == Qt::LeftButton) {
        mouseState.LeftButtonPressed = false;
        if (checkMouseSelection(e) == true) {
            print_debug(DEBUG_INTERFACE, "Registred object selection with left mouse.");
        }
    } else {
        mouseState.RightButtonPressed = false;
        if (checkMouseSelection(e) == true) {
            print_debug(DEBUG_INTERFACE, "Registred object selection with RIGHT mouse.");
            // Fall menu for selection
            QMenu menu(this);
            menu.addAction(actionManager->gotoAct);
            menu.addAction(actionManager->roomeditAct);
            menu.addAction(actionManager->deleteAct);
            menu.addAction(actionManager->deleteFullyAct);

            menu.exec(e->globalPos());
        }
    }
}

void MainWindow::mouseMoveEvent( QMouseEvent *e)
{
  QPoint pos;
  int dist_x, dist_y;

  pos = e->pos();
  dist_x = pos.x() - mouseState.oldPos.x();
  dist_y = pos.y() - mouseState.oldPos.y();
/*  
  print_debug(DEBUG_INTERFACE, "mouseEvent. LeftMouse %s, RightMouse %s. Dist_x %i, dist_y %i.",
      ON_OFF(LeftButtonPressed), ON_OFF(RightButtonPressed), dist_x, dist_y);
*/  
  if (mouseState.LeftButtonPressed) {
      renderer->userx += (float) dist_x / 10.0;
      renderer->usery -= (float) dist_y / 10.0;
      glredraw = 1;

      renderer->display();
      mouseState.oldPos = pos;
  } else if (mouseState.RightButtonPressed) {
    renderer->anglex += dist_y;
    renderer->angley += dist_x;
    glredraw = 1;
    
    
    renderer->display();
    mouseState.oldPos = pos;

  }

  
  
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
  int delta;

  delta = e->delta();

  renderer->userz += delta / 120;
  glredraw = 1;
  renderer->display();
}




