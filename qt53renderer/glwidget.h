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

#include "cbillboard.h"

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

private:
    bool prepareShaderProgram( const QString& vertexShaderPath,
                               const QString& fragmentShaderPath );

private:
    QOpenGLContext  *           m_context;
    QOpenGLTexture*             terrain_textures[16];

    QOpenGLFunctions_3_3_Core * m_func330;
    QOpenGLShaderProgram        m_shader;

    CBillboardsCollection   *   m_billboards;

    float                       xRot, yRot, zRot;



    GLuint                      _positionAttr;
    GLuint                      _colourAttr;
    GLuint                      _matrixAttr;
};

#endif // GLWIDGET_H
