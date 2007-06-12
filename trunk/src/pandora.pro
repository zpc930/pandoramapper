
TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CONFIG		+= qt opengl warn_on thread
CONFIG		+= release
CONFIG		-= debug

QT += xml opengl gui network

FORMS +=	configedit.ui
FORMS +=        roomedit.ui
FORMS +=	spellsdialog.ui
FORMS +=	movementdialog.ui
FORMS +=	logdialog.ui
FORMS +=	finddialog.ui


HEADERS		+=configurator.h
HEADERS		+=CRoom.h
HEADERS		+=Regions.h
HEADERS		+=defines.h
HEADERS		+=dispatch.h
HEADERS		+=engine.h
HEADERS		+=exits.h
HEADERS		+=forwarder.h
HEADERS		+=renderer.h
HEADERS		+=Map.h
HEADERS		+=stacks.h
HEADERS		+=tree.h
HEADERS		+=userfunc.h
HEADERS		+=utils.h
HEADERS		+=xml2.h
HEADERS		+=RoomEditDialog.h
HEADERS		+=ConfigWidget.h
HEADERS		+=SpellsDialog.h
HEADERS		+=mainwindow.h
HEADERS		+=Frustum.h
HEADERS		+=CSquare.h
HEADERS		+=CSelectionManager.h
HEADERS         +=CEvent.h
HEADERS         +=CActionManager.h
HEADERS         +=CMovementDialog.h
HEADERS         +=CLogDialog.h
HEADERS         +=finddialog.h



SOURCES		+=auda.cpp
SOURCES		+=CRoom.cpp
SOURCES		+=Regions.cpp
SOURCES		+=configurator.cpp
SOURCES		+=dispatch.cpp
SOURCES		+=engine.cpp
SOURCES		+=exits.cpp
SOURCES		+=forwarder.cpp
SOURCES		+=renderer.cpp
SOURCES		+=Map.cpp
SOURCES		+=stacks.cpp
SOURCES		+=tree.cpp
SOURCES		+=userfunc.cpp
SOURCES		+=utils.cpp
SOURCES		+=xml2.cpp
SOURCES		+=mainwindow.cpp
SOURCES		+=RoomEditDialog.cpp
SOURCES		+=ConfigWidget.cpp
SOURCES		+=SpellsDialog.cpp
SOURCES		+=Frustum.cpp
SOURCES         +=CSquare.cpp
SOURCES         +=CSelectionManager.cpp
SOURCES         +=CActionManager.cpp
SOURCES         +=CMovementDialog.cpp
SOURCES         +=CLogDialog.cpp
SOURCES         +=finddialog.cpp

TARGET		= ../pandora

macx {
	LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
}

win32:LIBS	+= -lwsock32
unix:LIBS		+= -lm 
!debug {
	unix:QMAKE_POST_LINK=strip $(TARGET)
}

SVN_REVISION=$$system("svn info | grep Revision | sed s/Revision:\ //")
!isEmpty(SVN_REVISION) {
	DEFINES += SVN_REVISION=$$SVN_REVISION
}

#CFLAGS_VAR	= $$system(pkg-config --cflags OGRE)
#CLIBS_VAR	= $$system(pkg-config --libs OGRE)
QMAKE_CXXFLAGS_RELEASE	+=  -O2 -pipe $$CFLAGS_VAR
QMAKE_CXXFLAGS_DEBUG	+=  $$CFLAGS_VAR
#LIBS 		+= $$CLIBS_VAR