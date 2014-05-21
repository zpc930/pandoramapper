#include "glwidget.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QOpenGLTexture>

typedef void (*PglGenVertexArrays) (GLsizei n,  GLuint *arrays);
typedef void (*PglBindVertexArray) (GLuint array);


GLWidget::GLWidget(QWindow *parent )
    : QWindow( parent ),
      m_context( 0 ),
      xRot(0.0), yRot(0.0), zRot(0.0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

GLWidget::~GLWidget()
{
    if (m_billboards)
        delete m_billboards;
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


    // Prepare a complete shader program...
    if ( !prepareShaderProgram( ":/simple.vert", ":/simple.frag" ) )
        return;

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }


    m_billboards = new CBillboardsCollection;
    m_billboards->add(4, 0, 0, "bla", false);
    m_billboards->add(-2, -2, 0, "bla");

    m_shader.release();

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
    m.perspective(60, 4.0/3.0, 0.1, 100.0);
    m.translate(0, 0, -15);



    //    QMatrix4x4 m;
    //    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 45.0f);
    //    m.translate(0.0f, 0.0f, -35.0f);
    //    m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    //    m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    //    m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);

    if (!m_shader.bind()) {
        qDebug() << "failed to bind shader";
    }
    m_shader.setUniformValue("matrix", m);

    terrain_textures[0]->bind();
    m_billboards->draw(m_shader);

    m_shader.release();

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

bool GLWidget::prepareShaderProgram( const QString& vertexShaderPath,
                                     const QString& fragmentShaderPath )
{
    qDebug() << "Compiling shaders...";

    // First we load and compile the vertex shader...
    bool result = m_shader.addShaderFromSourceFile( QOpenGLShader::Vertex, vertexShaderPath );
    if ( !result )
        qWarning() << m_shader.log();


    // ...now the fragment shader...
    result = m_shader.addShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShaderPath );
    if ( !result )
        qWarning() << m_shader.log();

    // ...and finally we link them to resolve any references.
    result = m_shader.link();
    if ( !result )
        qWarning() << "Could not link shader program:" << m_shader.log();

    return result;
}
