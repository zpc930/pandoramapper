#ifndef CBILLBOARD_H
#define CBILLBOARD_H

#include <string>
#include <vector>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class CBillboardsCollection
{

public:
    CBillboardsCollection();
    ~CBillboardsCollection();


    void draw(QOpenGLShaderProgram &shader);
    void add(float x, float y, float z, std::string text, bool rebuild = true);
    void rebuild();

private:
    void addTriangleToVerticesVector(std::vector<float> &vec, const QVector3D &a, const QVector3D &b, const QVector3D &c);
    void addBillboardQuad(float x, float y, float z, float width, float height);


private:
    std::vector<float>       m_billboardVertices;
    std::vector<float>       m_billboardTexCoord;
    QOpenGLVertexArrayObject m_billboardVao;
    QOpenGLBuffer            m_billboardVerticesBuf;
    QOpenGLBuffer            m_billboardTexCoordBuf;

};

#endif // CBILLBOARD_H
