#include "cbillboard.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

CBillboardsCollection::CBillboardsCollection(QOpenGLShaderProgram &_shader) :
    shader(_shader)
{
    m_billboardVao.create();
    m_billboardVao.bind();

    // texture coordinates
    m_billboardTexCoordBuf.create();
    m_billboardTexCoordBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_billboardTexCoordBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }

    shader.setAttributeBuffer( "texCoord", GL_FLOAT, 0, 2 );
    shader.enableAttributeArray( "texCoord" );


    m_billboardTexCoordBuf.release();

    // vertices
    m_billboardVerticesBuf.create();
    m_billboardVerticesBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_billboardVerticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }


    shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
    shader.enableAttributeArray( "vertex" );

    m_billboardVerticesBuf.release();

    m_billboardVao.release();
}


CBillboardsCollection::~CBillboardsCollection()
{
    m_billboardTexCoordBuf.destroy();
    m_billboardVerticesBuf.destroy();

    m_billboardVertices.clear();
    m_billboardTexCoord.clear();
}


void CBillboardsCollection::draw()
{
    //m.translate(0, 0, 0);

    // now draw billboards
    m_billboardVao.bind();
    glDrawArrays( GL_TRIANGLES, 0, m_billboardVertices.size() );
    m_billboardVao.release();
}


void CBillboardsCollection::add(float x, float y, float z, std::string text, bool _rebuild)
{
    addBillboardQuad(x, y, z, 5, 5);
    if (_rebuild)
        rebuild();
}

void CBillboardsCollection::rebuild()
{
    m_billboardVao.bind();

    if ( !m_billboardTexCoordBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }    m_billboardTexCoordBuf.allocate( &m_billboardTexCoord[0], m_billboardTexCoord.size() * 2 * sizeof( float ) );
    m_billboardTexCoordBuf.release();

    if ( !m_billboardVerticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_billboardVerticesBuf.allocate( &m_billboardVertices[0], m_billboardVertices.size() * 4 * sizeof( float ) );
    m_billboardVerticesBuf.release();
    m_billboardVao.release();
}


void CBillboardsCollection::addTriangleToVerticesVector(std::vector<float> &vec, const QVector3D &a, const QVector3D &b, const QVector3D &c)
{
    // vertice
    vec.push_back(a.x());
    vec.push_back(a.y());
    vec.push_back(a.z());
    vec.push_back(1.0f);

    vec.push_back(b.x());
    vec.push_back(b.y());
    vec.push_back(b.z());
    vec.push_back(1.0f);

    vec.push_back(c.x());
    vec.push_back(c.y());
    vec.push_back(c.z());
    vec.push_back(1.0f);
}

// x, y, z - point of the upper left corner
void CBillboardsCollection::addBillboardQuad(float x, float y, float z, float width, float height)
{
    QVector3D leftUpper(x, y, z);
    QVector3D leftLower(x, y + height, z);
    QVector3D rightUpper(x + width, y, z);
    QVector3D rightLower(x + width, y + height, z);

    addTriangleToVerticesVector(m_billboardVertices, leftLower, rightLower, leftUpper);
    addTriangleToVerticesVector(m_billboardVertices, leftUpper, rightLower, rightUpper);

    // push texture coordinates for 6 vertices
    m_billboardTexCoord.push_back(0.0f); // leftLower
    m_billboardTexCoord.push_back(1.0f); // leftLower

    m_billboardTexCoord.push_back(1.0f); // rightLower
    m_billboardTexCoord.push_back(1.0f); // rightLower

    m_billboardTexCoord.push_back(0.0f); // leftUpper
    m_billboardTexCoord.push_back(0.0f); // leftUpper

    m_billboardTexCoord.push_back(0.0f); // leftUpper
    m_billboardTexCoord.push_back(0.0f); // leftUpper

    m_billboardTexCoord.push_back(1.0f); // rightLower
    m_billboardTexCoord.push_back(1.0f); // rightLower

    m_billboardTexCoord.push_back(1.0f); // rightUpper
    m_billboardTexCoord.push_back(0.0f); // rightUpper
}

