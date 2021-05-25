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
    vec3 projectedPositions[DirLightCascadeMapCount];
    float shadowFactors[DirLightCascadeMapCount + 1];
    shadowFactors[DirLightCascadeMapCount] = 1.0;

    const vec2 textureSplitSize = vec2(1.01, 0.99) / DirLightCascadeMapCount;
    for (int i = 0; i < DirLightCascadeMapCount; i++)
    {
        vec4 textureLimitsXY = vec4(vec2(i, i + 1) * textureSplitSize, 0.001, 0.999);
        vec4 fragLightSpace = light.transform[i] * position;
        projectedPositions[i] = fragLightSpace.xyz / fragLightSpace.w;
        shadowFactors[i] = calcShadowFactor2D(projectedPositions[i], shadowMap, textureLimitsXY, 0.002);
    }

    float totalFactor = 1.0f;
    bool cascadeFound = false;
    for (int i = 0; i < DirLightCascadeMapCount; i++)
    {
        vec3 pos = projectedPositions[i];
        vec3 normCoords = abs(2.0 * pos - 1.0);
        normCoords = clamp(10.0 * normCoords - 9.0, 0.0, 1.0);
        float mixCoef = max(max(normCoords.x, normCoords.y), normCoords.z);
        float currentFactor = mix(shadowFactors[i], shadowFactors[i + 1], mixCoef);

        currentFactor = min(currentFactor, 1.0f);
        totalFactor = totalFactor * currentFactor;
    }

    return totalFactor;
}