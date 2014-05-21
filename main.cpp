#include <QApplication>
#include <QGLFormat>

#include "glwidget.h"

int main( int argc, char* argv[] )
{
    Q_INIT_RESOURCE(pandora);

    QGuiApplication a( argc, argv );

    GLWidget w;
    w.resize(400, 400);
    w.show();

    return a.exec();
}
