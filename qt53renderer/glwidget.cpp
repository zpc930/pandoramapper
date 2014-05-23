#include "glwidget.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QOpenGLTexture>

typedef void (*PglGenVertexArrays) (GLsizei n,  GLuint *arrays);
typedef void (*PglBindVertexArray) (GLuint array);


GLWidget::GLWidget(QWindow *parent )
    : QWindow( parent ),
      m_context( 0 ),
      m_billboards( 0 ),
      m_rooms( 0 ),
      xRot(0.0), yRot(0.0), zRot(0.0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

GLWidget::~GLWidget()
{
    if (m_billboards)
        delete m_billboards;
    if (m_rooms)
        delete m_rooms;
}


void GLWidget::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    resizeGL(size.width(), size.height());
}


void GLWidget::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
    {
        if (!m_context)
        {
            qDebug() << "creating initial context";

            m_context = new QOpenGLContext(this);
            QSurfaceFormat format(requestedFormat());
            format.setVersion(3,3);
            format.setDepthBufferSize(24);

            m_context->setFormat(format);
            m_context->create();

            m_context->makeCurrent(this);
            initializeOpenGLFunctions();

            m_func330 = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();
            if (m_func330)
                m_func330->initializeOpenGLFunctions();
            else
            {
                qWarning() << "Could not obtain required OpenGL context version";
                exit(1);
            }

            // get context opengl-version
            //qDebug() << "Widget OpenGl: " << m_context->majorVersion() << "." << format().minorVersion();
            //qDebug() << "Context valid: " << m_context()->isValid();
            //qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
            qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
            qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
            qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
            qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

            initGL();
        }

        paintGL();
    }
}

void GLWidget::initGL()
{
    qDebug() << "initGL";

    for (int j=0; j < 5; ++j) {

        QString texName = QString(":/images/side%1.png").arg(j + 1);
        qDebug() << "Loading texture: " << texName;
        terrain_textures[j] = new QOpenGLTexture( QImage(texName).mirrored() );
        terrain_textures[j]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        terrain_textures[j]->setMagnificationFilter(QOpenGLTexture::Linear);
    }

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    m_billboards = new CBillboardsCollection();
    m_billboards->add(4, 0, 0, "bla", false);
    m_billboards->add(-2, -2, 0, "bla");


    m_rooms = new CRoomCollectionRenderingElement();
//    m_rooms->add(0, 0, 0, 0, false);
//    m_rooms->add(0, 2, 0, 0, false);
    m_rooms->add(0, 0, 0, 0);


    resizeGL(width(), height());
}



void GLWidget::resizeGL( int w, int h )
{
    if (!m_context) // not yet initialized
        return;

    qDebug() << "resize event";
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLWidget::paintGL()
{
    if (!m_context) // not yet initialized
        return;

    m_context->makeCurrent(this);

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QMatrix4x4 m;
    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    m.translate(0, 0, -15);



    //    QMatrix4x4 m;
    //    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 45.0f);
    //    m.translate(0.0f, 0.0f, -35.0f);
    //    m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    //    m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    //    m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);


//    terrain_textures[0]->bind();
//    m_billboards->draw(m);

    terrain_textures[1]->bind();
    m_rooms->draw(m);

    m_context->swapBuffers(this);
    m_context->doneCurrent();
}

void GLWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
    case Qt::Key_Escape:
        QCoreApplication::instance()->quit();
        break;

    default:
        QWindow::keyPressEvent( e );
    }
}

