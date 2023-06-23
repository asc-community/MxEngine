#include "Library/shader_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;
layout(binding = 1) uniform sampler2D inputGhost;
layout(binding = 2) uniform sampler2D inputSceneAverageWhite;

uniform float uGhostDispersal; // dispersion factor
uniform float uHaloWidth;
uniform int uMipLevel;

in vec2 TexCoord;
out vec4 outputColor;

const float gWeight = 80.0f;

void main()
{
    float ghostAverageWhite = calcLuminance(texture(inputGhost, vec2(0.5), uMipLevel).rgb);
    if (ghostAverageWhite < texture(inputSceneAverageWhite,vec2(0.0)).r / 2.f)
    {
        outputColor = vec4(vec3(0.f), 1.0f);
        return;
    }

    vec3 haloColor = vec3(0.f);
    vec2 ghostTexcoord = vec2(1.0) - TexCoord;
    vec2 haloVec = normalize((vec2(0.5) - ghostTexcoord) * uGhostDispersal) * uHaloWidth;
    float weight = length(vec2(0.5) - fract(ghostTexcoord + haloVec)) / length(vec2(0.5));
    weight = pow(1.0 - weight, gWeight);
    haloColor = texture(inputColor, ghostTexcoord + haloVec).rgb * weight;

    outputColor = vec4(haloColor, 1.0f);
}