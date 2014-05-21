#version 330

layout(location = 0, index = 0) out vec4 fragColor;
in vec2 texc;

uniform sampler2D texture;

void main( void )
{
    fragColor = texture2D( texture, texc );
//    fragColor = texture2D( texture, vec2(0.5, 0.5) );
//    fragColor = vec4( 1.0f, 1.0f, 0.5f, 1.0f);
}
