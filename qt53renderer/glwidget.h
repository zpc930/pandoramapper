#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include <QGLWidget>

#include <QWindow>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLTexture>

#include <QBasicTimer>

#include "cbillboard.h"
#include "croomrenderingelement.h"

class GLWidget : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget( QWindow* parent = 0 );
    virtual ~GLWidget();

    void initGL();
    void resizeGL( int w, int h );
    void paintGL();


protected:
    virtual void exposeEvent(QExposeEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent( QKeyEvent* e );
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);

private:
    QOpenGLContext             *m_context;
    QOpenGLTexture*             terrain_textures[16];

    QOpenGLFunctions_3_3_Core * m_func330;

    CBillboardsCollection   *   m_billboards;
    CRoomCollectionRenderingElement * m_rooms;

    float                       xRot, yRot, zRot;

    QMatrix4x4                  m_projection;


    QBasicTimer                 timer;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;
};

#endif // GLWIDGET_H
