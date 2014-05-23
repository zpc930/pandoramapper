#include "croomrenderingelement.h"

CRoomCollectionRenderingElement::CRoomCollectionRenderingElement():
    CRenderingElement(":/room_shader.vert", ":/room_shader.frag")
{
    m_vao.create();
    m_vao.bind();

//    // texture coordinates
//    m_texCoordBuf.create();
//    m_texCoordBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
//    if ( !m_texCoordBuf.bind() )
//    {
//        qWarning() << "Could not bind vertex buffer to the context";
//        return;
//    }

//    m_shader.setAttributeBuffer( "texCoord", GL_FLOAT, 0, 2 );
//    m_shader.enableAttributeArray( "texCoord" );
//    m_texCoordBuf.release();

    // vertices
    m_verticesBuf.create();
    m_verticesBuf.setUsagePattern( QOpenGLBuffer::StaticDraw );
    if ( !m_verticesBuf.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }


    m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4, sizeof(float) * 6 );
    m_shader.enableAttributeArray( "vertex" );

    int offset = sizeof(QVector4D);
    qDebug() << "offset is: " << offset;

    m_shader.setAttributeBuffer( "texCoord", GL_FLOAT, offset, 2, sizeof(float) * 6 );
    m_shader.enableAttributeArray( "texCoord" );


//    // Offset for position
//    quintptr offset = 0;

//    // Tell OpenGL programmable pipeline how to locate vertex position data
//    int vertexLocation = m_shader.attributeLocation("a_position");
//    m_shader.enableAttributeArray(vertexLocation);
//    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void *)offset);

//    // Offset for texture coordinate
//    offset += sizeof(QVector3D);
//    //4 * sizeof(float);
//    qDebug() << "offset is: " << offset;

//    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
//    int texcoordLocation = m_shader.attributeLocation("a_texcoord");
//    m_shader.enableAttributeArray(texcoordLocation);
//    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void *)offset);



    m_verticesBuf.release();

    m_vao.release();

    m_shader.release();
}

CRoomCollectionRenderingElement::~CRoomCollectionRenderingElement()
{
    m_verticesBuf.destroy();
    m_vertices.clear();

    //m_texCoordBuf.destroy();
    //m_texCoord.clear();
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
    QVector4D leftUpper(x, y, z, 1.0f);
    QVector4D leftLower(x, y + height, z, 1.0f);
    QVector4D rightUpper(x + width, y, z, 1.0f);
    QVector4D rightLower(x + width, y + height, z, 1.0f);

//    addTriangleToVerticesVector(m_vertices, leftLower, rightLower, leftUpper);
    m_vertices.push_back(leftLower.x());
    m_vertices.push_back(leftLower.y());
    m_vertices.push_back(leftLower.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(0.0f); // leftLower
    m_vertices.push_back(1.0f); // leftLower

//    m_vertices.push_back(VertexData(leftLower, QVector2D(0.0, 1.0)));


    m_vertices.push_back(rightLower.x());
    m_vertices.push_back(rightLower.y());
    m_vertices.push_back(rightLower.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(1.0f); // rightLower
    m_vertices.push_back(1.0f); // rightLower

//    m_vertices.push_back(VertexData(rightLower, QVector2D(1.0, 1.0)));

    m_vertices.push_back(leftUpper.x());
    m_vertices.push_back(leftUpper.y());
    m_vertices.push_back(leftUpper.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(0.0f); // leftUpper
    m_vertices.push_back(0.0f); // leftUpper

//    m_vertices.push_back(VertexData(leftUpper, QVector2D(0.0, 0.0)));


//    addTriangleToVerticesVector(m_vertices, leftUpper, rightLower, rightUpper);
    m_vertices.push_back(leftUpper.x());
    m_vertices.push_back(leftUpper.y());
    m_vertices.push_back(leftUpper.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(0.0f); // leftUpper
    m_vertices.push_back(0.0f); // leftUpper

//    m_vertices.push_back(VertexData(leftUpper, QVector2D(0.0, 0.0)));


    m_vertices.push_back(rightLower.x());
    m_vertices.push_back(rightLower.y());
    m_vertices.push_back(rightLower.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(1.0f); // rightLower
    m_vertices.push_back(1.0f); // rightLower

//    m_vertices.push_back(VertexData(rightLower, QVector2D(1.0, 1.0)));


    m_vertices.push_back(rightUpper.x());
    m_vertices.push_back(rightUpper.y());
    m_vertices.push_back(rightUpper.z());
    m_vertices.push_back(1.0f);
    m_vertices.push_back(1.0f); // rightUpper
    m_vertices.push_back(0.0f); // rightUpper

//    m_vertices.push_back(VertexData(rightUpper, QVector2D(1.0, 0.0)));

}


void CRoomCollectionRenderingElement::draw(QMatrix4x4 &projMatrix)
{
    //m.translate(0, 0, 0);

    if (!m_shader.bind()) {
        qDebug() << "failed to bind shader";
    }
    m_shader.setUniformValue("matrix", projMatrix);

    // now draw billboards
    m_vao.bind();

    m_verticesBuf.bind();

    glDrawArrays( GL_TRIANGLES, 0, m_vertices.size() * 6 );


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
        float p = *it;
        //qDebug() << "[ " << p.position << "," << p.texCoord << "]";
        qDebug() << p;

    }


    m_verticesBuf.allocate( &m_vertices[0], m_vertices.size() * 6 * sizeof( float ) );
    m_verticesBuf.release();
    m_vao.release();
}
