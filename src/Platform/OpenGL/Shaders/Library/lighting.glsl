#include "Library/shader_utils.glsl"

vec3 calculateLighting(vec3 fragmentColor, vec3 fragmentNormal, vec3 lightDirection, vec3 lightColor, float ambientFactor, float shadowFactor)
{
    float NL = max(dot(normalize(lightDirection), fragmentNormal), 0.0f);

    vec3 diffuseColor = NL * shadowFactor * fragmentColor;
    vec3 ambientColor = ambientFactor * fragmentColor;

    return (ambientColor + diffuseColor) * lightColor;
}
