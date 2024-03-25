#include "Library/fragment_utils.glsl"

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
    float sceneAverageWhite = texture(inputSceneAverageWhite,vec2(0.0)).r;
    float factor = smoothstep(0.0,1.0,(ghostAverageWhite/sceneAverageWhite)-0.5);

    vec2 ghostTexcoord = vec2(1.0) - TexCoord;
    vec2 haloVec = normalize((vec2(0.5) - ghostTexcoord) * uGhostDispersal) * uHaloWidth;
    float weight = length(vec2(0.5) - fract(ghostTexcoord + haloVec)) / length(vec2(0.5));  
    weight = pow(1.0 - weight, gWeight);
    vec3 haloColor = texture(inputColor, ghostTexcoord + haloVec).rgb * weight;

    outputColor = vec4(haloColor * factor, 1.0f);
}