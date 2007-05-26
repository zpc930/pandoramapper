
#include <QFileDialog>
#include <QMessageBox>


#include "defines.h"

#include "mainwindow.h"
#include "utils.h"
#include "stacks.h"
#include "engine.h"
#include "userfunc.h"
#include "forwarder.h"
#include "CActionManager.h"


CActionManager::CActionManager(MainWindow *parentWindow) 
{
    parent = parentWindow;

    /* creating actions and connecting them here */
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new map"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));    
    
    openAct = new QAction(tr("&Load..."), this);
    openAct->setShortcut(tr("Ctrl+L"));
    openAct->setStatusTip(tr("Open an existing map"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    
    reloadAct = new QAction(tr("&Reload..."), this);
    reloadAct->setShortcut(tr("Ctrl+R"));
    reloadAct->setStatusTip(tr("Reload current map"));
    connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));
    
    saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save currently opened map"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    
    saveAsAct = new QAction(tr("Save As..."), this);
    saveAsAct->setStatusTip(tr("Save the map As"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    
    publishAct= new QAction(tr("Clear secrets"), this);
    publishAct->setStatusTip(tr("Removes all secret exits and rooms behind them"));
    connect(publishAct, SIGNAL(triggered()), this, SLOT(publish_map()));    
    
        
    quitAct =  new QAction(tr("&Exit..."), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit Pandora"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
        
    /* Room menu */
    roomeditAct= new QAction(tr("Edit"), this);
    roomeditAct->setStatusTip(tr("View/Edit current Room's info"));
    connect(roomeditAct, SIGNAL(triggered()), this, SLOT(edit_current_room()));    
    
    deleteAct= new QAction(tr("Delete"), this);
    deleteAct->setStatusTip(tr("Deletes this room"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(delete_room()));    

    deleteFullyAct= new QAction(tr("Delete Fully"), this);
    deleteFullyAct->setStatusTip(tr("Deletes this room and everything that leads to it, including doors in other rooms"));
    connect(deleteFullyAct, SIGNAL(triggered()), this, SLOT(deleteFully()));    

    
    mergeAct= new QAction(tr("Merge"), this);
    mergeAct->setStatusTip(tr("Tries to merge two twin rooms"));
    connect(mergeAct, SIGNAL(triggered()), this, SLOT(merge_room()));    
    
    /* Mapping menu */
    
    mappingAct= new QAction(tr("Mapping"), this);
    mappingAct->setCheckable(true);
    mappingAct->setChecked(false);
    mappingAct->setStatusTip(tr("Switch mapping mode on/off"));
    connect(mappingAct, SIGNAL(triggered()), this, SLOT(mapping_mode()));    
    
    automergeAct= new QAction(tr("AutoMerge"), this);
    automergeAct->setCheckable(true);
    conf->get_automerge() ? automergeAct->setChecked(true) : automergeAct->setChecked(false);
    automergeAct->setStatusTip(tr("Automatically merge twin (same name/desc) rooms"));
    connect(automergeAct, SIGNAL(triggered()), this, SLOT(automerge()));    
    
    angryLinkerAct= new QAction(tr("AngryLinker"), this);
    angryLinkerAct->setCheckable(true);
    conf->get_angrylinker() ? angryLinkerAct->setChecked(false) : angryLinkerAct->setChecked(false);
    angryLinkerAct->setStatusTip(tr("Auto-link neightbour rooms"));
    connect(angryLinkerAct, SIGNAL(triggered()), this, SLOT(angrylinker()));    
    
  
    
    /* Configuration menu bar */
    
    always_on_top_action= new QAction(tr("Always on Top"), this);
    always_on_top_action->setStatusTip(tr("Always on Top"));
    always_on_top_action->setCheckable(true);
    connect(always_on_top_action, SIGNAL(toggled(bool)), this, SLOT(always_on_top(bool)), Qt::QueuedConnection);
    always_on_top_action->setChecked(conf->get_always_on_top());
    
    
    
    emulationAct= new QAction(tr("Emulation Mode"), this);
    emulationAct->setStatusTip(tr("Offline MUME Emulation"));
    emulationAct->setCheckable(true);
    emulationAct->setChecked(false);
    connect(emulationAct, SIGNAL(triggered()), this, SLOT(emulation_mode()));    
    
    
    setupGeneralAct= new QAction(tr("General Settings ..."), this);
    setupGeneralAct->setStatusTip(tr("Edit general settings"));
    connect(setupGeneralAct, SIGNAL(triggered()), this, SLOT(generalSetting()) );    
    
    spellsAct= new QAction(tr("Spells Settings"), this);
    spellsAct->setStatusTip(tr("Spells Settings"));
    connect(spellsAct, SIGNAL(triggered()), this, SLOT(spellsSettings()) );    
    
    
    saveConfigAct= new QAction(tr("Save Configuration ..."), this);
    saveConfigAct->setStatusTip(tr("Save current configuration"));
    connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(saveConfig()));    
    
    saveConfigAsAct= new QAction(tr("Save Configuration As ..."), this);
    saveConfigAsAct->setStatusTip(tr("Save current configuration to a different file"));
    connect(saveConfigAsAct, SIGNAL(triggered()), this, SLOT(saveAsConfig()));    
    
    loadConfigAct= new QAction(tr("Load Configuration"), this);
    loadConfigAct->setStatusTip(tr("Save current configuration to a different file"));
    connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(loadConfig()));    
    
    /* Help menu. */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    
    // Additional Context menu actions
    gotoAct = new QAction(tr("Goto"), this);
    connect(gotoAct, SIGNAL(triggered()), this, SLOT(gotoAction()));
}


void CActionManager::edit_current_room()
{
    unsigned int id;

    if (Map.selections.isEmpty() == false) {
        id = Map.selections.getFirst();
    } else {
        if (stacker.amount() != 1) {
            QMessageBox::critical(parent, "Room Info Edit", QString("You are not in sync!"));
            return;
        } 
        id = stacker.first()->id;
    }
    
    parent->editRoomDialog( id );
}

void CActionManager::spellsSettings()
{
    if (!parent->spells_dialog) {
        parent->spells_dialog = new SpellsDialog(parent);
    }

    parent->spells_dialog->run();
    parent->spells_dialog->show();
    parent->spells_dialog->raise();
    parent->spells_dialog->activateWindow();
}

void CActionManager::generalSetting()
{
    if (!parent->generalSettingsDialog) {
        parent->generalSettingsDialog = new ConfigWidget(parent);
    }

    parent->generalSettingsDialog->run();
    parent->generalSettingsDialog->show();
    parent->generalSettingsDialog->raise();
    parent->generalSettingsDialog->activateWindow();
}


void CActionManager::about()
{
    QMessageBox::about(parent, tr("About Pandora"),
            tr("<h2>Pandora MUME Mapper</h2>"
               "<p>Copyright &copy; 2003-2007 Azazello and contributors."
               "<p>This is an automatic mapper and database software for "
               "a game named MUME. It uses MUME's XML for parsing the "
               "output of the game to get user's position on the map and "
               "represents it in 3d view using OpenGL."
               "<p>Visit <a href=\"http://code.google.com/p/pandoramapper/\">code.google.com/p/pandoramapper/</a> for more information."));
}




void CActionManager::always_on_top(bool set_on_top)
{
  print_debug(DEBUG_INTERFACE, "always_on_top called");

  Qt::WindowFlags flags = parent->windowFlags();
  if( set_on_top )
  {
    flags |= Qt::WindowStaysOnTopHint;
    print_debug(DEBUG_INTERFACE, "setting WindowStaysOnTopHint");
  } else {
    flags &= ~Qt::WindowStaysOnTopHint;
    print_debug(DEBUG_INTERFACE, "clearing WindowStaysOnTopHint");
  }
  parent->setWindowFlags(flags);
  parent->show();
  conf->set_always_on_top(set_on_top);
}



void CActionManager::newFile()
{
    
}


void CActionManager::disable_online_actions()
{
    mappingAct->setEnabled(false);
    automergeAct->setEnabled(false);
    angryLinkerAct->setEnabled(false);
}


void CActionManager::enable_online_actions()
{
    mappingAct->setEnabled(true);
    automergeAct->setEnabled(true);
    angryLinkerAct->setEnabled(true);
}


void CActionManager::delete_room()
{
    if (Map.selections.size() > 1) {
        int ret = QMessageBox::warning(parent, tr("Pandora: Room's Deletion"),
                   tr("Do you really want to delete this room/rooms?"),
                   QMessageBox::Ok |  QMessageBox::Cancel,
                   QMessageBox::Ok);


        if (ret == QMessageBox::Cancel) 
            return;
    }
//    while (Map.selections.isEmpty() != true)
        userland_parser->parse_user_input_line("mdelete");
}

void CActionManager::deleteFully()
{
    int ret = QMessageBox::warning(parent, tr("Pandora: Room's Deletion"),
                   tr("Do you really want to delete this room/rooms?\n"
                       "Note that this deletion will also remove all\n"
                        "exits and doors leading to this room from all\n"
                        "other rooms"),
                   QMessageBox::Ok |  QMessageBox::Cancel,
                   QMessageBox::Ok);

    if (ret == QMessageBox::Cancel) 
        return;
//    while (Map.selections.isEmpty() == false)
        userland_parser->parse_user_input_line("mdelete remove");
}


void CActionManager::merge_room()
{
    userland_parser->parse_user_input_line("mmerge");
}

void CActionManager::open()
{
  QString s = QFileDialog::getOpenFileName(
                    parent,
                    "Choose a database",
                    "database/",
                    "XML files (*.xml)");    
  char data[MAX_STR_LEN];
    
  strcpy(data, qPrintable(s));
    
  if (!s.isEmpty()) { 
    usercmd_mload(0, 0,  data, data);  
  }  
  QMessageBox::information(parent, "Pandora", "Loaded!\n", QMessageBox::Ok);

}

void CActionManager::reload()
{
    userland_parser->parse_user_input_line("mload");
}

void CActionManager::quit()
{
    if (conf->get_data_mod()) {
        switch(QMessageBox::information(parent, "Pandora",
                                        "The map contains unsaved changes\n"
                                        "Do you want to save the changes before exiting?",
                                        "&Save", "&Discard", "Cancel",
                                        0,      // Enter == button 0
                                        2)) { // Escape == button 2
        case 0: // Save clicked or Alt+S pressed or Enter pressed.
            save();
            break;
        case 1: // Discard clicked or Alt+D pressed
            // don't save but exit
            break;
        case 2: // Cancel clicked or Escape pressed
            return;// don't exit
            break;
        }    
    
    } 
    
    if (conf->get_conf_mod()) {
        switch(QMessageBox::information(parent, "Pandora",
                                        "The configuration was changed\n"
                                        "Do you want to write it down on disc before exiting?",
                                        "&Save", "&Discard", "Cancel",
                                        0,      // Enter == button 0
                                        2)) { // Escape == button 2
        case 0: // Save clicked or Alt+S pressed or Enter pressed.
            conf->save_config();
            break;
        case 1: // Discard clicked or Alt+D pressed
            // don't save but exit
            break;
        case 2: // Cancel clicked or Escape pressed
            return;// don't exit
            break;
        }    
    
    
    }
    QApplication::quit();
}

void CActionManager::save()
{
    userland_parser->parse_user_input_line("msave");
    QMessageBox::information(parent, "Saving...", "Saved!\n", QMessageBox::Ok);
}

void CActionManager::saveAs()
{
  char data[MAX_STR_LEN];
  
  QString s = QFileDialog::getSaveFileName(
                    parent,
                    "Choose a filename to save under",
                    "database/",
                    "XML database files (*.xml)");
                    
  strcpy(data, qPrintable(s));
    
  if (!s.isEmpty()) { 
    usercmd_msave(0, 0,  data, data);  
  }  
  
  QMessageBox::information(parent, "Saving...", "Saved!\n", QMessageBox::Ok);
}


void CActionManager::mapping_mode()
{
    if (mappingAct->isChecked()) 
    {
        userland_parser->parse_user_input_line("mmap on");
    } else {
        userland_parser->parse_user_input_line("mmap off");
    }
}

void CActionManager::automerge()
{
    if (automergeAct->isChecked()) 
    {
        userland_parser->parse_user_input_line("mautomerge on");
    } else {
        userland_parser->parse_user_input_line("mautomerge off");
    }
}

void CActionManager::angrylinker()
{
    if (angryLinkerAct->isChecked()) 
    {
        userland_parser->parse_user_input_line("mangrylinker on");
    } else {
        userland_parser->parse_user_input_line("mangrylinker off");
    }
}

void CActionManager::saveConfig()
{
    conf->save_config();
    QMessageBox::information(parent, "Saving...", "Saved!\n", QMessageBox::Ok);
}

void CActionManager::saveAsConfig()
{
    QString s = QFileDialog::getSaveFileName(
                    parent,
                    "Choose a filename to save under",
                    "configs/",
                    "XML config files (*.xml)");
    if (s.isEmpty())
        return;
    conf->save_config_as("", s.toAscii());
    QMessageBox::information(parent, "Saving...", "Saved!\n", QMessageBox::Ok);
}

void CActionManager::loadConfig()
{
  QString s = QFileDialog::getOpenFileName(
                    parent,
                    "Choose another configuration file to load",
                    "configs/",
                    "XML config files (*.xml)");    
  if (s.isEmpty())
        return;

  conf->load_config("", s.toAscii());
  QMessageBox::information(parent, "Pandora", "Loaded!\n", QMessageBox::Ok);

}    

void CActionManager::emulation_mode()
{
    if (emulationAct->isChecked()) {
        /* cannot turn it on if we have anyone connected to us already */
        /* tricky check - if online_actions are on, then we have connections */
        if (mappingAct->isChecked()) {
            emulationAct->setChecked(false); 
            QMessageBox::critical(parent, "Pandora",
                              QString("You have to disconnect from the game first!"));
            return;        
        }
        proxy->setMudEmulation( true );
        engine->setPrompt("-->");
        stacker.put(1);
        stacker.swap();
    } else {
        proxy->setMudEmulation( false );
    
    }

}


void CActionManager::publish_map()
{
    bool mark[MAX_ROOMS];
    CRoom *r;
    unsigned int i;
    unsigned int z;
    
    if (mappingAct->isChecked()) {
        emulationAct->setChecked(false); 
        QMessageBox::critical(parent, "Pandora",
                          QString("You have to disconnect first!"));
        return;        
    }
        
    memset(mark, 0, MAX_ROOMS);
    stacker.reset();
    stacker.put(1);
    stacker.swap();
    while (stacker.amount() != 0) {
        for (i = 0; i < stacker.amount(); i++) {
            r = stacker.get(i);
            mark[r->id] = true;
            for (z = 0; z <= 5; z++) {
                if (r->isConnected(z) && mark[ r->exits[z]->id  ] != true ) {
                    if ( r->isDoorSecret(z) == true  ) {
                        stacker.put(r->exits[z]->id);
                    }
                }
            }
        }
        stacker.swap();
    }
    
    
    for (i = 0; i < Map.size(); i++) {
        r = Map.rooms[i];
        if (r) {
            if (!mark[r->id]) {
                Map.deleteRoom(r, 0);
                continue;        
            }
        }
        
    }
    
    for (i = 0; i < Map.size(); i++) {
        r = Map.rooms[i];
        if (r) {
            for (z = 0; z <= 5; z++) {
                if ( r->isDoorSecret(z) == true ) {
                    print_debug(DEBUG_ROOMS,"Secret door was still in database...\r\n");
                    r->removeDoor(z);
                }
            }
        }
        
    }

    
    
    
    
    
    print_debug(DEBUG_INTERFACE && DEBUG_ROOMS,"Finished removing secrets from the map!\r\n");
    //    QMessageBox::information(parent, "Removing secrets...", "Done!\n", QMessageBox::Ok);
}



void CActionManager::gotoAction() {
    userland_parser->parse_user_input_line("mgoto");
}
