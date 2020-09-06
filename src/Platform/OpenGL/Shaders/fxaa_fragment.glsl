#include "fxaa.glsl"
EMBEDDED_SHADER(

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;

void main()
{
    OutColor = fxaa(tex, TexCoord).rgba;
}

)