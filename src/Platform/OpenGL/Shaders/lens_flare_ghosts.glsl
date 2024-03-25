#include "Library/fragment_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;
layout(binding = 1) uniform sampler2D averageWhiteTex;

uniform float uScale;
uniform float uBias;

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
    vec3 col = texture(inputColor, TexCoord).rgb;
	float sceneLum = calcLuminance(col);
	float averageWhite = texture(averageWhiteTex, vec2(0.0f)).r;

    float lumDiff = sceneLum - averageWhite;
    col = max(lumDiff, 0.0f) * col;
    outputColor = vec4(col, 1.0f);
}