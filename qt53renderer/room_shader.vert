#version 330

uniform mat4 mvp_matrix;

in vec4 vertex;
in vec2 texCoord;
out vec2 texc;



void main( void )
{
    gl_Position = mvp_matrix * vertex;
    texc = texCoord;
}
