#include "Library/fxaa.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;

void main()
{
    OutColor = vec4(fxaa(tex, TexCoord).rgb, 1.0f);
}