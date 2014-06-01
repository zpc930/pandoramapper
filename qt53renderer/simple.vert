#version 330

attribute vec3 vertex;
in vec2 texCoord;
out vec2 texc;

uniform highp mat4 matrix;


void main( void )
{
    gl_Position = matrix * vec4(vertex, 1.0);
    texc = texCoord;
}
