
QT       += core gui opengl widgets

TARGET = 01-core-profile
TEMPLATE = app

SOURCES +=  \
            main.cpp \
            glwidget.cpp \
    cbillboard.cpp \
    croomrenderingelement.cpp \
    crenderingelement.cpp

HEADERS  += \
            glwidget.h \
    cbillboard.h \
    croomrenderingelement.h \
    crenderingelement.h

OTHER_FILES +=  \
                simple.vert \
                simple.frag

RESOURCES +=     pandora.qrc
