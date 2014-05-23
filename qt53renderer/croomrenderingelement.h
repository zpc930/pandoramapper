#ifndef CROOMRENDERINGELEMENT_H
#define CROOMRENDERINGELEMENT_H


#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "crenderingelement.h"

struct VertexData
{
public:
    VertexData(const QVector4D &pos, const QVector3D &tex) :
        position(pos), texCoord(tex)
    {}

    QVector4D position;
    QVector2D texCoord;
};

class CRoomCollectionRenderingElement : public CRenderingElement
{

public:
    CRoomCollectionRenderingElement();
    ~CRoomCollectionRenderingElement();


    void add(float x, float y, float z, int type, bool _rebuild = true);

    virtual void draw(QMatrix4x4 &projMatrix);
    virtual void rebuild();

private:
    void addQuad(float x, float y, float z, float width, float height);

private:
//    std::vector<VertexData>    m_vertices;
    std::vector<float>       m_vertices;

//    std::vector<float>       m_vertices;
//    std::vector<float>       m_texCoord;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_verticesBuf;
//    QOpenGLBuffer            m_texCoordBuf;
};

#endif // CROOMRENDERINGELEMENT_H
