#include "crenderingelement.h"


CRenderingElement::CRenderingElement(QString vertexShader, QString fragmentShader)
{
    // Prepare a complete shader program...
    if ( !prepareShaderProgram( vertexShader, fragmentShader ) ) {
        qDebug() << "Failed to properly create Rendering Element";
        return;
    }

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }
}


bool CRenderingElement::prepareShaderProgram( const QString& vertexShaderPath,
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

