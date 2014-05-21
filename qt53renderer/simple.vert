#version 330

in vec4 vertex;
in vec2 texCoord;
out vec2 texc;

uniform highp mat4 matrix;


void main( void )
{
    gl_Position = matrix * vertex;
    texc = texCoord;
}
