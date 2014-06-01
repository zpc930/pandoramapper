#ifndef CRENDERINGELEMENT_H
#define CRENDERINGELEMENT_H


#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class CRenderingElement {

public:
    CRenderingElement(QString vertexShader, QString fragmentShader);

    virtual void draw(QMatrix4x4 &projMatrix) { Q_UNUSED(projMatrix); }
    virtual void rebuild() {}

    bool prepareShaderProgram( const QString& vertexShaderPath,
                               const QString& fragmentShaderPath );



protected:
    QOpenGLShaderProgram    m_shader;
};

#endif // CRENDERINGELEMENT_H
