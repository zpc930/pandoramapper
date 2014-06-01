#include "croomrenderingelement.h"

CRoomCollectionRenderingElement::CRoomCollectionRenderingElement():
    CRenderingElement(":/room_shader.vert", ":/room_shader.frag")
{
    m_vao.create();
    m_vao.bind();

    // vertices
    m_verticesBuf.create();
    m_verticesBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_verticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }


    m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 3, sizeof(VertexData) /* sizeof(float) * 5 */ );
    m_shader.enableAttributeArray( "vertex" );

    int offset = sizeof(QVector3D);
    qDebug() << "offset is: " << offset;

    m_shader.setAttributeBuffer( "texCoord", GL_FLOAT, offset, 2, sizeof(VertexData) /*sizeof(float) * 5*/ );
    m_shader.enableAttributeArray( "texCoord" );


    m_verticesBuf.release();

    m_vao.release();

    m_shader.release();
}

CRoomCollectionRenderingElement::~CRoomCollectionRenderingElement()
{
    m_verticesBuf.destroy();
    m_vertices.clear();
}


void CRoomCollectionRenderingElement::add(float x, float y, float z, int type, bool _rebuild)
{
    addQuad(x - 1, y - 1, z, 2, 2);
    if (_rebuild)
        rebuild();
}



// x, y, z - point of the upper left corner
void CRoomCollectionRenderingElement::addQuad(float x, float y, float z, float width, float height)
{
    QVector3D leftUpper(x, y, z);
    QVector3D leftLower(x, y + height, z);
    QVector3D rightUpper(x + width, y, z);
    QVector3D rightLower(x + width, y + height, z);

    m_vertices.push_back(VertexData(leftLower, QVector2D(0.0, 1.0)));
    m_vertices.push_back(VertexData(rightLower, QVector2D(1.0, 1.0)));
    m_vertices.push_back(VertexData(leftUpper, QVector2D(0.0, 0.0)));
    m_vertices.push_back(VertexData(leftUpper, QVector2D(0.0, 0.0)));
    m_vertices.push_back(VertexData(rightLower, QVector2D(1.0, 1.0)));
    m_vertices.push_back(VertexData(rightUpper, QVector2D(1.0, 0.0)));
}


void CRoomCollectionRenderingElement::draw(QMatrix4x4 &projMatrix)
{
    //m.translate(0, 0, 0);

    if (!m_shader.bind()) {
        qDebug() << "failed to bind shader";
    }

    qDebug() << "using matrix: " << projMatrix;

    int matLoc = m_shader.uniformLocation("mvp_matrix");

    m_shader.setUniformValue(matLoc, projMatrix);


    // now draw billboards
    m_vao.bind();
    m_verticesBuf.bind();

    glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() * 5 );

    m_verticesBuf.release();
    m_vao.release();

    m_shader.release();
}


void CRoomCollectionRenderingElement::rebuild()
{
    m_vao.bind();
    if ( !m_verticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }

    qDebug() << "rebuilding rooms";

    for (auto it = m_vertices.begin(); it != m_vertices.end(); it++){
        auto p = *it;
        qDebug() << "[ " << p.position << "," << p.texCoord << "]";
        //qDebug() << p;

    }


    m_verticesBuf.allocate( &m_vertices[0], m_vertices.size() * sizeof(VertexData) );
    m_verticesBuf.release();
    m_vao.release();
}
