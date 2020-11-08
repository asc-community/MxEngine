#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, EnvironmentInfo environment, int GGXSamples, vec3 lightColor, float ambientFactor, float shadowFactor)
{
	vec3 normLightDirection = normalize(lightDirection);
    mat3 transformH = computeSampleTransform(fragment.normal);
    
    vec3 specularColor = vec3(0.0f);
    vec3 FKtotal = vec3(0.0f);

    float roughness = max(fragment.roughnessFactor, 0.01f);

    float invEnvironmentSampleCount = 1.0f / float(GGXSamples);
    float A = computeA(environment.skybox, GGXSamples);
    for (uint i = 0; i < uint(GGXSamples); i++)
    {
        vec3 H = GGXSample(sampleHammersley(i, invEnvironmentSampleCount), roughness * roughness);
        H = transformH * H;
        vec3 direction = reflect(-viewDirection, H);

        vec3 FK;
        float pdf;

        vec3 specularK = GGXCookTorranceSampled(fragment.normal, direction, viewDirection, roughness, fragment.metallicFactor, fragment.albedo, FK, pdf);
        FKtotal += FK;
        float lod = computeLOD(A, pdf, direction);

        vec3 sampleDirection = environment.skyboxRotation * direction;
        vec3 sampledColor = textureLod(environment.skybox, sampleDirection, lod).rgb;
        specularColor += specularK * sampledColor;
    }
    specularColor *= invEnvironmentSampleCount;
    FKtotal *= invEnvironmentSampleCount;
    vec3 irradianceColor = calcReflectionColor(environment.irradiance, environment.skyboxRotation, viewDirection, fragment.normal);
   
    vec3 diffuseColor = fragment.albedo * (1.0f - fragment.metallicFactor) * clamp(1.0f - FKtotal, 0.0f, 1.0f) * irradianceColor;
    vec3 ambientColor = diffuseColor * ambientFactor;

    diffuseColor *= max(dot(lightDirection, fragment.normal), 0.0f);
    specularColor *= max(dot(lightDirection, fragment.normal), 0.0f);

    vec3 totalColor = ambientColor + shadowFactor * (diffuseColor + specularColor);
    return totalColor * lightColor * fragment.ambientOcclusion;
}