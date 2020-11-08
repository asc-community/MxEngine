#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, EnvironmentInfo environment, vec3 lightColor, float ambientFactor, float shadowFactor)
{
	vec3 normLightDirection = normalize(lightDirection);
    mat3 transformH = computeSampleTransform(fragment.normal);
    
    vec3 specularColor = vec3(0.0f);
    vec3 FKtotal = vec3(0.0f);

    int environmentSamples = 16;
    float invEnvironmentSampleCount = 1.0f / float(environmentSamples);
    float A = computeA(environment.skybox, environmentSamples);
    for (uint i = 0; i < uint(environmentSamples); i++)
    {
        vec3 H = GGXSample(sampleHammersley(i, invEnvironmentSampleCount), fragment.roughnessFactor * fragment.roughnessFactor);
        H = transformH * H;
        vec3 direction = reflect(-viewDirection, H);

        vec3 FK;
        float pdf;

        vec3 specularK = GGXCookTorranceSampled(fragment.normal, direction, viewDirection, fragment.roughnessFactor, fragment.metallicFactor, fragment.albedo, FK, pdf);
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
    vec3 ambientColor = fragment.albedo * ambientFactor;

    diffuseColor *= clamp(shadowFactor,  0.7f, 1.0f);
    specularColor *= clamp(shadowFactor, 0.7f, 1.0f);

    vec3 totalColor = ambientColor + diffuseColor + specularColor;
    return totalColor * lightColor * fragment.ambientOcclusion;
}