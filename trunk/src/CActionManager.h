#ifndef CACTIONMANAGER_H
#define CACTIONMANAGER_H

#include <QAction>
#include <QMainWindow>
class MainWindow;

class CActionManager : public QObject {
    Q_OBJECT

    MainWindow *parent;
public:
    CActionManager(MainWindow *parent);

    QAction       *newAct;
    QAction       *openAct;
    QAction       *reloadAct;
    QAction       *saveAct;
    QAction       *saveAsAct;
    QAction       *quitAct;
    QAction       *publishAct;

    QAction       *mappingAct;
    QAction       *automergeAct;
    QAction       *angryLinkerAct;
    QAction       *duallinkerAct;

    QActionGroup  *toolsGroup;
    QAction       *selectToolAct;
    QAction       *mapMoveToolAct;
    QAction       *deleteToolAct;

    QAction       *roomeditAct;
    QAction       *deleteAct;
    QAction       *mergeAct;
    QAction       *moveRoomAct;

    QAction       *always_on_top_action;
    QAction       *saveConfigAct;
    QAction       *saveConfigAsAct;
    QAction       *loadConfigAct;
    QAction       *setupGeneralAct;
    QAction       *spellsAct;

    QAction       *showLogAct;

    QAction       *gotoAct;
    QAction       *deleteFullyAct;
    
    QAction       *aboutQtAct;
    QAction       *aboutAct;
    
    QAction       *emulationAct;

public slots:
    void disable_online_actions();
    void enable_online_actions();
    void updateActionsSettings();
    
    
    void always_on_top(bool);
    void newFile();
    void open();
    void reload();
    void quit();
    void save();
    void saveAs();

    void showLog();
    
    void mapping_mode();
    void automerge();
    void angrylinker();
    void duallinker();
    
    void delete_room(); 
    void merge_room(); 
    
    void saveConfig();
    void saveAsConfig();
    void loadConfig();
    
    void generalSetting();
    void spellsSettings();
    void edit_current_room();
    
    void emulation_mode();  
    void publish_map();
    void about();
    void gotoAction();
    void deleteFully(); // delete with remove flag
};


#endif
