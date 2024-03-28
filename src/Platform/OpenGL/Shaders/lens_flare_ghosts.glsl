#include "Library/fragment_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;
layout(binding = 1) uniform sampler2D averageWhiteTex;

uniform float uIntensity; // [0, 1] range

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
    vec3 col = texture(inputColor, TexCoord).rgb;
    float sceneLum = calcLuminance(col);

    float averageWhite = texture(averageWhiteTex, vec2(0.0)).r;
    float lumRatio = sceneLum / averageWhite;
    float lum = lumRatio < 1.0 ? 0.0 : pow(lumRatio, uIntensity) - 1.0;
    outputColor = vec4(lum * col, averageWhite);
}