#include "Library/shader_utils.glsl"
in vec2 TexCoord;
out vec4 OutColor;
layout(binding = 0)uniform sampler2D colorTex;
layout(binding = 1)uniform sampler2D bokehCocTex;

void main()
{
    vec3 color = vec3(0.0);
    vec4 info = texture(bokehCocTex, TexCoord, 0);
    float coc = info.a;
    vec3 bokeh = info.rgb;
    vec3 originColor = texture(colorTex, TexCoord, 0).rgb;
    color = mix(originColor, bokeh, coc);
    OutColor = vec4(color, 1.0);
}