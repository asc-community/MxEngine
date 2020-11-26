#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, vec3 lightColor, float ambientFactor, float shadowFactor)
{
    float roughness = clamp(fragment.roughnessFactor, 0.01f, 0.99f);
    float metallic = clamp(fragment.metallicFactor, 0.01f, 0.99f);

    vec3 H = normalize(lightDirection + viewDirection);
    vec3 direction = 2.0f * dot(viewDirection, H) * H - viewDirection;

    vec3 FK;
    float pdf;
    vec3 specularK = GGXCookTorranceSampled(fragment.normal, direction, viewDirection, roughness, metallic, fragment.albedo, FK, pdf);
    vec3 specularColor = specularK * lightColor;
    vec3 diffuseColor = fragment.albedo * (1.0f - metallic) * (vec3(1.0f) - FK) * lightColor;
    vec3 ambientColor = fragment.albedo* lightColor * ambientFactor;

    float NL = max(dot(fragment.normal, lightDirection), 0.0f);

    vec3 totalColor = ambientColor + (diffuseColor + specularColor) * NL * shadowFactor;
    return totalColor * fragment.ambientOcclusion;
}
