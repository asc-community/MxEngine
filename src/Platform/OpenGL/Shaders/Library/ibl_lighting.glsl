#include "Library/lighting.glsl"

vec3 calculateIBL(FragmentInfo fragment, vec3 viewDirection, EnvironmentInfo environment, int GGXSamples, float gamma)
{
    vec3 specularColor = vec3(0.0f);
    vec3 FKtotal = vec3(0.0f);

    float roughness = clamp(fragment.roughnessFactor, 0.01f, 0.99f);
    float metallic = clamp(fragment.metallicFactor, 0.01f, 0.99f);

    float invEnvironmentSampleCount = 1.0f / float(GGXSamples);
    float A = computeA(environment.skybox, GGXSamples);
    mat3 sampleTransform = computeSampleTransform(fragment.normal);
    for (uint i = 0; i < uint(GGXSamples); i++)
    {
        vec2 Xi = sampleHammersley(i, invEnvironmentSampleCount);
        vec3 H = GGXImportanceSample(Xi, roughness, fragment.normal, sampleTransform);
        vec3 direction = 2.0f * dot(viewDirection, H) * H - viewDirection;
        
        vec3 FK;
        float pdf;

        vec3 specularK = GGXCookTorranceSampled(fragment.normal, direction, viewDirection, roughness, metallic, fragment.albedo, FK, pdf);
        FKtotal += FK;
        float lod = computeLOD(A, pdf, direction);

        vec3 sampledColor = textureLod(environment.skybox, environment.skyboxRotation * direction, lod).rgb;
        specularColor += specularK * pow(sampledColor, vec3(gamma));
    }
    specularColor *= invEnvironmentSampleCount;
    FKtotal *= invEnvironmentSampleCount;
    vec3 irradianceColor = calcReflectionColor(environment.irradiance, environment.skyboxRotation, viewDirection, fragment.normal);
    irradianceColor = pow(irradianceColor, vec3(gamma));
    
    float diffuseCoef = 1.0f - metallic;
    vec3 diffuseColor = fragment.albedo * (irradianceColor - irradianceColor * FKtotal) * diffuseCoef;
    vec3 iblColor = (diffuseColor + specularColor) * environment.intensity;

    return fragment.emmisionFactor * fragment.albedo + iblColor * fragment.ambientOcclusion;
}

vec3 calculateIBL(FragmentInfo fragment, vec3 viewDirection, sampler2D envBRDFLUT, EnvironmentInfo environment, float gamma)
{
    float roughness = clamp(fragment.roughnessFactor, 0.01, 0.99);
    float metallic = clamp(fragment.metallicFactor, 0.01, 0.99);

    float NV = clamp(dot(fragment.normal, viewDirection), 0.0, 0.999);
    
    float lod = log2(textureSize(environment.skybox, 0).x * roughness * roughness);
    vec3 F0 = mix(vec3(0.04f), fragment.albedo, metallic);
    vec3 F = fresnelSchlickRoughness(F0, NV, roughness);

    vec3 prefilteredColor = calcReflectionColor(environment.skybox, environment.skyboxRotation, viewDirection, fragment.normal, lod);
    prefilteredColor = pow(prefilteredColor, vec3(gamma));
    vec2 envBRDF = texture2D(envBRDFLUT, vec2(NV, roughness)).rg;
    vec3 specularColor = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 irradianceColor = calcReflectionColor(environment.irradiance, environment.skyboxRotation, viewDirection, fragment.normal);
    irradianceColor = pow(irradianceColor, vec3(gamma));
    
    float diffuseCoef = 1.0f - metallic;
    vec3 diffuseColor = fragment.albedo * (irradianceColor - irradianceColor * envBRDF.y) * diffuseCoef;
    vec3 iblColor = (diffuseColor + specularColor) * environment.intensity;

    return fragment.emmisionFactor * fragment.albedo + iblColor * fragment.ambientOcclusion;
}