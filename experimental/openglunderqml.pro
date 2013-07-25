QT += qml quick widgets gui

HEADERS += squircle.h logorenderer.h fboinsgrenderer.h
SOURCES += squircle.cpp  logorenderer.cpp fboinsgrenderer.cpp main.cpp
RESOURCES += openglunderqml.qrc


target.path = $$[QT_INSTALL_EXAMPLES]/quick/scenegraph/openglunderqml
INSTALLS += target

OTHER_FILES += \
    main.qml \
    error.qml
