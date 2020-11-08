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

vec3 GGXSample(vec2 angles, float roughnessSquared)
{
    float phi = 2.0f * PI * angles.x;
    float alpha2 = roughnessSquared * roughnessSquared;
    float cosTheta = clamp(sqrt(1.0f - angles.y) / (1.0f + alpha2 * angles.y - angles.y), 0.0f, 1.0f);
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}

mat3 computeSampleTransform(vec3 normal)
{
    mat3 w;
    vec3 up = abs(normal.y) < 0.999f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
    w[0] = normalize(cross(up, normal));
    w[1] = cross(normal, w[0]);
    w[2] = normal;
    return w;
}

float computeA(samplerCube tex, int sampleCount)
{
    vec2 sizes = textureSize(tex, 0);
    float A = 0.5f * log2(sizes.x * sizes.y / float(sampleCount));
    return A;
}

float computeLOD(float A, float pdf, vec3 lightDirection)
{
    float du = 2.0f * 1.2f * abs(lightDirection.z + 1.0f);
    return max(0.0f, A - 0.5f * log2(pdf * du * du) + 1.0f);
}

float radicalInverseVDC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 sampleHammersley(uint i, float invSampleCount)
{
    return vec2(i * invSampleCount, radicalInverseVDC(i));
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

    vec3 trueAlbedo = mix(albedo, vec3(0.0f), metallic);
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    float G = GGXPartialGeometry(NV, roughness2) * GGXPartialGeometry(NL, roughness2);
    float D = GGXDistribution(NH, roughness2);
    vec3 F = fresnelSchlick(F0, HV);

    vec3 specular = G * D * F * 0.25f / NV;
    vec3 diffuse = clamp(vec3(1.0f) - F, vec3(0.0f), vec3(1.0f));
    return max(vec3(0.0f), trueAlbedo * diffuse * NL / PI + specular);
}

vec3 GGXCookTorranceSampled(vec3 normal, vec3 lightDirection, vec3 viewDirection, float roughness, float metallic, vec3 albedo, out vec3 FK, out float pdf)
{
    vec3 H = normalize(viewDirection + lightDirection);
    float roughness2 = roughness * roughness;
    float NV = dot(normal, viewDirection);
    float NL = dot(normal, lightDirection);
    float NH = dot(normal, H);
    float HV = dot(H, viewDirection);

    if (NV < 0.0f) return vec3(0.0f);
    if (NL < 0.0f) return vec3(0.0f);

    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    float G = GGXPartialGeometry(NV, roughness2) * GGXPartialGeometry(NL, roughness2);
    float D = GGXDistribution(NH, roughness2);
    vec3 F = fresnelSchlick(F0, HV);

    FK = F;
    pdf = D * NH / (4.0f * HV);
    
    vec3 specular = G * F * HV / (NV * NH);
    return max(vec3(0.0f), specular);
}