QT += qml quick widgets gui

QT += quick-private

HEADERS += squircle.h logorenderer.h Proxy\proxy.h Proxy\CDispatcher.h qtbroker.h mudoutput.h fboinsgrenderer.h
SOURCES += squircle.cpp  logorenderer.cpp fboinsgrenderer.cpp Proxy\proxy.cpp Proxy\CDispatcher.cpp mudoutput.cpp main.cpp
RESOURCES += openglunderqml.qrc

DEFINES += NOMINMAX

INCLUDEPATH = ./

target.path = $$[QT_INSTALL_EXAMPLES]/quick/scenegraph/openglunderqml
INSTALLS += target

OTHER_FILES += \
    main.qml \
    error.qml


macx:LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
win32:LIBS += -lwsock32
unix:LIBS += -lm
