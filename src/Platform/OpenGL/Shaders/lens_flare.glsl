#include "Library/fragment_utils.glsl"
layout(binding = 0) uniform sampler2D inputGhost;
layout(binding = 1) uniform sampler2D inputHalo;
layout(binding = 2) uniform sampler2D inputColor;

uniform int uGhosts; // number of ghost samples
uniform float uGhostDispersal; // dispersion factor

in vec2 TexCoord;
out vec4 outColor;

const float gWeight = 80.0f;//relative to pixel pos

void main()
{
    vec2 texcoord = vec2(1.0) - TexCoord;
    // ghost vector to image centre:
    vec2 ghostVec = (vec2(0.5) - texcoord) * uGhostDispersal;

    // sample ghosts
    vec3 result = vec3(0.0);
    for (int i = 0; i < uGhosts; ++i)
    {
        vec2 offset = fract(texcoord + 0.8 * ghostVec * float(i));
        float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
        weight = pow(1.0 - weight, gWeight);

        result += texture(inputGhost, offset).rgb * weight;
    }

    //halo
    result += texture(inputHalo, TexCoord).rgb;

    outColor = vec4(result + texture(inputColor, TexCoord).rgb, 1.f);
}