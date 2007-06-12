#include <QtGui>

#include "mainwindow.h"
#include "utils.h"
#include "stacks.h"
#include "engine.h"
#include "userfunc.h"
#include "forwarder.h"
#include "CActionManager.h"

void toggle_renderer_reaction()
{
    if (renderer_window->renderer->redraw == false) {
        renderer_window->renderer->redraw = true;
        proxy->startRendererCall();
    }
}

void notify_analyzer()
{
    proxy->startEngineCall();
}

/*  globals end */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow( parent)
{
    spells_dialog = NULL;
    edit_dialog = NULL;
    generalSettingsDialog = NULL;
    movementDialog = NULL;
    logdialog = NULL;
    findDialog = NULL;
    mapMoveMode = false;
    deleteMode = false;

    userland_parser = new Userland();
    actionManager = new CActionManager(this);
    
    print_debug(DEBUG_INTERFACE, "in mainwindow constructor");
    
    setWindowTitle("Pandora");
    renderer =  new RendererWidget( this );
    setCentralWidget( renderer );
    
    setGeometry( conf->get_window_rect() );
    
    connect(proxy, SIGNAL(connectionEstablished()), actionManager, SLOT(enable_online_actions()), Qt::QueuedConnection );
    connect(proxy, SIGNAL(connectionLost()), actionManager, SLOT(disable_online_actions()), Qt::QueuedConnection );

    /* Enable mouse tracking to be able to show tooltips. */
    setMouseTracking(true);


    /* now building a menu and adding actions to menu */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(actionManager->newAct);
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->openAct);  
    fileMenu->addAction(actionManager->reloadAct);  
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->saveAct);  
    fileMenu->addAction(actionManager->saveAsAct);  
    fileMenu->addSeparator();
    fileMenu->addAction(actionManager->quitAct);  


    actionsMenu = menuBar()->addMenu(tr("&Room"));
    actionsMenu->addAction(actionManager->findAct);
    actionsMenu->addAction(actionManager->roomeditAct);
    actionsMenu->addAction(actionManager->moveRoomAct);
    actionsMenu->addAction(actionManager->deleteAct);
    actionsMenu->addAction(actionManager->deleteFullyAct);
    actionsMenu->addAction(actionManager->mergeAct);
    actionsMenu->addAction(actionManager->refreshAct);

    
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(actionManager->selectToolAct);
    toolsMenu->addAction(actionManager->mapMoveToolAct);
    toolsMenu->addAction(actionManager->deleteToolAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(actionManager->selectionTypeAct);



    
    mappingMenu = menuBar()->addMenu(tr("&Mapping"));
    mappingMenu->addAction(actionManager->mappingAct);
    mappingMenu->addAction(actionManager->automergeAct);
    mappingMenu->addAction(actionManager->angryLinkerAct);
    mappingMenu->addAction(actionManager->duallinkerAct);
    mappingMenu->addSeparator();
    mappingMenu->addAction(actionManager->publishAct);

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
    //optionsMenu->addAction(actionManager->loadConfigAct);



    logMenu = menuBar()->addMenu(tr("&Log") );
    logMenu->addAction(actionManager->showLogAct );

    menuBar()->addSeparator();

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
    connect(conf, SIGNAL(configurationChanged()),
            actionManager, SLOT(updateActionsSettings() ), Qt::QueuedConnection);
}


void MainWindow::moveRoomDialog()
{
    print_debug(DEBUG_INTERFACE, "move room dialog action called");

    // check if there is an objective for this operation
    if (Map.selections.size() == 0 && stacker.amount() != 1) {
        QMessageBox::critical(this, "Movement Dialog",
                             QString("You have to either get in sync or select some rooms!"));
        return;
    }

    // create the dialog if needed
    if (!movementDialog) {
        movementDialog = new CMovementDialog (this);
    }

    // launch the dialog
    movementDialog->show();
    movementDialog->raise();
    movementDialog->activateWindow();

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

    QString modLabel;
    QString firstPart;
    
    if (conf->get_data_mod() )
        firstPart = "Data: MOD ";
    else 
        firstPart = "Data: --- ";

    modLabel = QString("Rooms Selected %1 ").arg( Map.selections.size() );   

    emit newModLabel(modLabel + firstPart);     
    
    stacker.getCurrent(str);
    emit newLocationLabel(str);
    print_debug(DEBUG_INTERFACE, "Done updating interface!\r\n");
}

/* Reimplement main even handler to catch tooltip events. */
bool MainWindow::event(QEvent *event)
{
    unsigned int id;

    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        if (renderer->doSelect( mousePosInRenderer( helpEvent->pos() ), id ))
            QToolTip::showText(helpEvent->globalPos(), Map.getRoom(id)->toolTip());
        else
#if QT_VERSION >= 0x040200
            QToolTip::hideText();
#else
            QToolTip::showText(QPoint(), QString());
#endif
    }
    return QWidget::event(event);
}

void MainWindow::keyPressEvent( QKeyEvent *k )
{

    print_debug(DEBUG_INTERFACE, "Processing events in keyPressEvent\r\n");
    switch ( k->key() ) {
         
        case Qt::Key_X :
            renderer->userz += 1;
            toggle_renderer_reaction();
            break;
    
         case Qt::Key_Y:
            renderer->userz -= 1;
            toggle_renderer_reaction();
            break;
    
         case Qt::Key_Q:
            renderer->userx -= 1;
            toggle_renderer_reaction();
            break;
         
         case Qt::Key_W:
            renderer->userx += 1;
            toggle_renderer_reaction();
            break;
         
         case Qt::Key_A:
            renderer->usery += 1;
            toggle_renderer_reaction();
            break;
    
         case Qt::Key_S:
            renderer->usery -= 1;
            toggle_renderer_reaction();
            break;
    
        case Qt::Key_Up:
            renderer->anglex += 5;
            toggle_renderer_reaction();
            break;
        case Qt::Key_Down:
            renderer->anglex -= 5;
            toggle_renderer_reaction();
            break;
        case Qt::Key_Left:
            renderer->angley -= 5;
            toggle_renderer_reaction();
            break;
        case Qt::Key_Right:
            renderer->angley += 5;
            toggle_renderer_reaction();
            break;
        case Qt::Key_PageUp:
            renderer->anglez += 5;
            toggle_renderer_reaction();
            break;
        case Qt::Key_PageDown:
            renderer->anglez -= 5;
            toggle_renderer_reaction();
            break;
        
        case Qt::Key_Plus:
            renderer->changeUserLayerShift( +1 );
            toggle_renderer_reaction();
            break;

        case Qt::Key_Minus:
            renderer->changeUserLayerShift( -1 );
            toggle_renderer_reaction();
            break;
        
         case Qt::Key_Escape:        
            printf("Escape pressed!\r\n");   
            renderer->angley = 0;
            renderer->anglex = 0;
            renderer->anglez = 0;
            renderer->userx = 0;
            renderer->usery = 0;
            renderer->userz = BASE_Z;	
            renderer->userLayerShift = 0;
            toggle_renderer_reaction();
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
         case Qt::Key_Space:
            if (!k->isAutoRepeat()) {
                setMapMoveMode(true);
            }
            break;
    }
    update_status_bar();
    renderer->display();
    print_debug(DEBUG_INTERFACE, "Done processing events at keyEventPress\r\n");
}

void MainWindow::keyReleaseEvent( QKeyEvent *k )
{

    print_debug(DEBUG_INTERFACE, "Processing events in keyReleaseEvent\r\n");
    switch ( k->key() ) {
        case Qt::Key_Space:
            if (!k->isAutoRepeat()) {
                setMapMoveMode(false);
            }
            break;
    }
}

void MainWindow::mousePressEvent( QMouseEvent *e )
{

    mouseState.oldPos = e->pos();
    mouseState.origPos = e->pos();

    if (e->button() == Qt::LeftButton)
        mouseState.LeftButtonPressed = true;
    else if (e->button() == Qt::RightButton)
        mouseState.RightButtonPressed = true;

    if (mapMoveMode) {
#if QT_VERSION >= 0x040200
        renderer->setCursor(Qt::ClosedHandCursor);
#endif
    } else if (deleteMode) {
        if (e->button() == Qt::LeftButton) {
            if (checkMouseSelection(e) == true) {
                print_debug(DEBUG_INTERFACE, "Deleting room.");
                userland_parser->parse_user_input_line("mdelete");
            }
        }
    } else {
        /* Select rooms if not in map moving mode. */
        if (e->button() == Qt::LeftButton) {
            if (checkMouseSelection(e) == true) {
                print_debug(DEBUG_INTERFACE, "Registered object selection with left mouse button.");
            }
        } else if (e->button() == Qt::RightButton) {
            if (checkMouseSelection(e) == true) {
                print_debug(DEBUG_INTERFACE, "Registered object selection with right mouse button.");

                createContextMenu(e);
            }
        }
    }
}

/*
 * FIXME: The dialogs that pop up from context menu get rooms from selection.
 * If more than one room was previously selected, strange results may happen.
 */
void MainWindow::createContextMenu( QMouseEvent *e )
{
    unsigned int id;
    QAction *roomNameAct;
    QMenu menu(this);
    QString roomName;

    if (renderer->doSelect( mousePosInRenderer( e->pos() ), id )) {
        roomName = Map.getRoom(id)->getName();
    } else {
        roomName = tr("No room here");
    }

    /* Trim room name to 25 characters. */
    if (roomName.length() > 25)
        roomName = roomName.left(22) + "...";

    roomNameAct = new QAction(roomName, this);
    roomNameAct->setEnabled(false);
    menu.addAction(roomNameAct);
    menu.addSeparator();
    menu.addAction(actionManager->gotoAct);
    menu.addAction(actionManager->roomeditAct);
    menu.addAction(actionManager->moveRoomAct);
    menu.addAction(actionManager->deleteAct);
    menu.addAction(actionManager->deleteFullyAct);
    menu.addAction(actionManager->refreshAct);

    menu.addAction(actionManager->bindRoomsAct);
    if (Map.selections.size() == 2)
        actionManager->bindRoomsAct->setEnabled(true);
    else 
        actionManager->bindRoomsAct->setEnabled(false);

    menu.exec(e->globalPos());
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
            if (e->modifiers() & Qt::ControlModifier) {
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

void MainWindow::mouseReleaseEvent( QMouseEvent *e )
{

    if (e->button() == Qt::LeftButton) {
        mouseState.LeftButtonPressed = false;
    } else {
        mouseState.RightButtonPressed = false;
    }

#if QT_VERSION >= 0x040200
    if (mapMoveMode)
        renderer->setCursor(Qt::OpenHandCursor);
#endif
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

    if (mapMoveMode) {
        if (mouseState.LeftButtonPressed) {
            renderer->userx += (float) dist_x / 10.0;
            renderer->usery -= (float) dist_y / 10.0;
            toggle_renderer_reaction();

            mouseState.oldPos = pos;
        } else if (mouseState.RightButtonPressed) {
            renderer->anglex += dist_y;
            renderer->angley += dist_x;

            toggle_renderer_reaction();
            mouseState.oldPos = pos;
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    int delta;
    
    delta = e->delta();
    
    renderer->userz += delta / 120;
    toggle_renderer_reaction();
}

void MainWindow::setMapMoveMode(bool b)
{
    if (mapMoveMode != b)
        emit mapMoveValueChanged(b);

    mapMoveMode = b;

    if (mapMoveMode) {
#if QT_VERSION >= 0x040200
        renderer->setCursor(Qt::OpenHandCursor);
#endif
    } else {
        renderer->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::setDeleteMode(bool b)
{
    deleteMode = b;

    if (deleteMode) {
        renderer->setCursor(Qt::CrossCursor);
    } else {
        renderer->setCursor(Qt::ArrowCursor);
    }
}