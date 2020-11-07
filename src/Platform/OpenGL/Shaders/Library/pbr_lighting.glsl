// https://m.habr.com/ru/post/326852/

#define PI 3.1415926535f

float GGXPartialGeometry(float normalLightAngle, float roughnessSquared)
{
    float alpha2 = roughnessSquared * roughnessSquared;
    float theta2 = clamp(normalLightAngle * normalLightAngle, 0.0f, 1.0f);
    float tan2 = (1.0f - theta2) / theta2;
    float GP = 2.0f / (1.0f + sqrt(1.0f + alpha2 * tan2));
    return GP;
}

float GGXDistribution(float normalLightAngle, float roughnessSquared)
{
    float alpha2 = roughnessSquared * roughnessSquared;
    float theta2 = clamp(normalLightAngle * normalLightAngle, 0.0f, 1.0f);
    float distr = theta2 * alpha2 + (1.0f - theta2);
    return alpha2 / (PI * distr * distr);
}

vec3 fresnelSchlick(vec3 angles, float normalLightAngle)
{
    return angles + (vec3(1.0f) - angles) * pow(1.0f - clamp(normalLightAngle, 0.0f, 1.0f), 5.0f);
}

vec3 GGXCookTorrance(vec3 normal, vec3 lightDirection, vec3 viewDirection, float roughness, float metallic, vec3 albedo)
{
    vec3 H = normalize(viewDirection + lightDirection);
    float roughness2 = roughness * roughness;
    float NV = dot(normal, viewDirection);
    float NL = dot(normal, lightDirection);
    float NH = dot(normal, H);
    float HV = dot(H, viewDirection);

    if (NV < 0.0f) return vec3(0.0f);
    if (NL < 0.0f) return vec3(0.0f);

    const vec3 luminance = vec3(0.2125f, 0.7154f, 0.0721f);
    vec3 trueAlbedo = mix(albedo, vec3(0.0f), metallic);
    vec3 F0 = mix(vec3(dot(luminance, albedo)), albedo, metallic);

    float G = GGXPartialGeometry(NV, roughness2) * GGXPartialGeometry(NL, roughness2);
    float D = GGXDistribution(NH, roughness2);
    vec3 F = fresnelSchlick(F0, HV);

    vec3 specular = G * D * F * 0.25f / NV;
    vec3 diffuse = clamp(vec3(1.0f) - F, vec3(0.0f), vec3(1.0f));
    return max(vec3(0.0f), trueAlbedo * diffuse * NL / PI + specular);
}