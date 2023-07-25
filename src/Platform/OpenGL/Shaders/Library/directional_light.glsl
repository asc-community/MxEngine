#include "Library/ibl_lighting.glsl"

const int DirLightCascadeMapCount = 3;
const int MaxDirLightCount = 4;

struct DirLight
{
    mat4 transform[DirLightCascadeMapCount];
    vec4 color;
    vec3 direction;
};

float calcShadowFactorCascade(vec4 position, DirLight light, sampler2D shadowMap)
{
    const vec2 TEXTURE_BIAS = vec2(0.001, 0.001);
    const float invCascadeCount = 1.0 / DirLightCascadeMapCount;
    vec3 depthBias = vec3(0.0005, 0.0003, 0.0002);
    for (int i = 0; i < DirLightCascadeMapCount; i++)
    {
        vec2 textureLimitsX = vec2(i, i + 1) * invCascadeCount + vec2(TEXTURE_BIAS.x, -TEXTURE_BIAS.x);
        vec4 textureLimitsXY = vec4(textureLimitsX, TEXTURE_BIAS.y, 1.0 - TEXTURE_BIAS.y);
        vec4 fragLightSpace = light.transform[i] * position;
        vec3 projectedPosition = fragLightSpace.xyz / fragLightSpace.w;
        float s = calcShadowFactor2D(projectedPosition, shadowMap, textureLimitsXY, depthBias[i]);
        if (s != -1.0) return s;
    }
    return 1.0;
}