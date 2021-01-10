#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, vec3 lightColor, float ambientFactor, float shadowFactor)
{
    float roughness = clamp(fragment.roughnessFactor, 0.01f, 0.99f);
    float metallic = clamp(fragment.metallicFactor, 0.01f, 0.99f);

    vec3 H = normalize(normalize(lightDirection) + viewDirection);
    vec3 direction = reflect(viewDirection, H);

    vec3 FK;
    float pdf;
    vec3 specularColor = GGXCookTorranceSampled(fragment.normal, direction, viewDirection, roughness, metallic, fragment.albedo, FK, pdf);

    float diffuseCoef = 1.0f - metallic;
    vec3 diffuseColor = fragment.albedo * (diffuseCoef - diffuseCoef * FK);
    vec3 ambientColor = fragment.albedo * ambientFactor;

    float NL = clamp(dot(fragment.normal, lightDirection), 0.0, 1.0);

    float shadowCoef = NL * shadowFactor;

    vec3 totalColor = (ambientColor + (diffuseColor + specularColor) * shadowCoef) * lightColor;
    return totalColor * fragment.ambientOcclusion;
}
