#ifndef CBILLBOARD_H
#define CBILLBOARD_H

#include <string>
#include <vector>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "crenderingelement.h"



class CBillboardsCollection : public CRenderingElement
{
public:
    CBillboardsCollection();
    ~CBillboardsCollection();

    virtual void draw(QMatrix4x4 &projMatrix);
    virtual void rebuild();

    void add(float x, float y, float z, std::string text, bool rebuild = true);

private:
    void addTriangleToVerticesVector(std::vector<float> &vec, const QVector3D &a, const QVector3D &b, const QVector3D &c);
    void addBillboardQuad(float x, float y, float z, float width, float height);


private:
    std::vector<float>       m_vertices;
    std::vector<float>       m_texCoord;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_verticesBuf;
    QOpenGLBuffer            m_texCoordBuf;
};

#endif // CBILLBOARD_H
