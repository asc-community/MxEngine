#include "Library/shader_utils.glsl"

layout(binding = 0) uniform sampler2D inputColor;
layout(binding = 1) uniform sampler2D inputGhost;

uniform float uGhostDispersal; // dispersion factor
uniform float uHaloWidth;

in vec2 TexCoord;

out vec4 outputColor;

void main()
{
    vec3 haloColor = vec3(0.f);

    vec2 ghostTexcoord = vec2(1.0) - TexCoord;
    vec2 haloVec = normalize((vec2(0.5) - ghostTexcoord) * uGhostDispersal) * uHaloWidth;
    float weight = length(vec2(0.5) - fract(ghostTexcoord + haloVec)) / length(vec2(0.5));
    weight = pow(1.0 - weight, 80.f);
    haloColor = texture(inputColor, ghostTexcoord + haloVec).rgb * weight;

    if (calcLuminance(texture(inputGhost, vec2(0.5), 4).rgb) < 5)
    {
        outputColor = vec4(vec3(0.f), 1.0f);
        return;
    }
    outputColor = vec4(haloColor, 1.0f);
}