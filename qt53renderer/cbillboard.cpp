#include "cbillboard.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

CBillboardsCollection::CBillboardsCollection() :
    CRenderingElement(":/simple.vert", ":/simple.frag")
{
    m_vao.create();
    m_vao.bind();

    // texture coordinates
    m_texCoordBuf.create();
    m_texCoordBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_texCoordBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }

    m_shader.setAttributeBuffer( "texCoord", GL_FLOAT, 0, 2 );
    m_shader.enableAttributeArray( "texCoord" );


    m_texCoordBuf.release();

    // vertices
    m_verticesBuf.create();
    m_verticesBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_verticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }


    m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
    m_shader.enableAttributeArray( "vertex" );

    m_verticesBuf.release();

    m_vao.release();

    m_shader.release();
}


CBillboardsCollection::~CBillboardsCollection()
{
	m_vertices.clear();
	m_texCoord.clear();
    
	m_texCoordBuf.destroy();
    m_verticesBuf.destroy();
}


void CBillboardsCollection::draw(QMatrix4x4 &projMatrix)
{
    //m.translate(0, 0, 0);

    if (!m_shader.bind()) {
        qDebug() << "failed to bind shader";
    }
    m_shader.setUniformValue("matrix", projMatrix);


    // now draw billboards
    m_vao.bind();
    glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() );
    m_vao.release();

    m_shader.release();
}


void CBillboardsCollection::add(float x, float y, float z, std::string text, bool _rebuild)
{
    addBillboardQuad(x, y, z, 5, 5);
    if (_rebuild)
        rebuild();
}

void CBillboardsCollection::rebuild()
{
    m_vao.bind();

    if ( !m_texCoordBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }    m_texCoordBuf.allocate( &m_texCoord[0], m_texCoord.size() * 2 * sizeof( float ) );
    m_texCoordBuf.release();

    if ( !m_verticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_verticesBuf.allocate( &m_vertices[0], m_vertices.size() * 4 * sizeof( float ) );
    m_verticesBuf.release();
    m_vao.release();
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

    addTriangleToVerticesVector(m_vertices, leftLower, rightLower, leftUpper);
    addTriangleToVerticesVector(m_vertices, leftUpper, rightLower, rightUpper);

    // push texture coordinates for 6 vertices
    m_texCoord.push_back(0.0f); // leftLower
    m_texCoord.push_back(1.0f); // leftLower

    m_texCoord.push_back(1.0f); // rightLower
    m_texCoord.push_back(1.0f); // rightLower

    m_texCoord.push_back(0.0f); // leftUpper
    m_texCoord.push_back(0.0f); // leftUpper

    m_texCoord.push_back(0.0f); // leftUpper
    m_texCoord.push_back(0.0f); // leftUpper

    m_texCoord.push_back(1.0f); // rightLower
    m_texCoord.push_back(1.0f); // rightLower

    m_texCoord.push_back(1.0f); // rightUpper
    m_texCoord.push_back(0.0f); // rightUpper
}

