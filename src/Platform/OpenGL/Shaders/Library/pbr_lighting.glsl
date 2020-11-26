// https://m.habr.com/ru/post/326852/

#define PI 3.1415926535f

float GGXPartialGeometry(float NV, float roughness)
{
    float k = (roughness + 1.0f) * (roughness + 1.0f) * 0.125f;
    return NV / (NV * (1.0f - k) + k);
}

float GGXDistribution(float NH, float roughness)
{
    float roughness2 = roughness * roughness;
    float alpha2 = roughness2 * roughness2;
    float distr = (NH * NH) * (alpha2 - 1.0f) + 1.0f;
    float distr2 = distr * distr;
    float totalDistr = alpha2 / (PI * distr2);
    return totalDistr;
}

float GGXSmith(float NV, float NL, float roughness)
{
    return GGXPartialGeometry(NV, roughness)* GGXPartialGeometry(NL, roughness);
}

vec3 fresnelSchlick(vec3 F0, float HV)
{
    return F0 + (1.0f - F0) * pow(1.0f - HV, 5.0f);
}

mat3 computeSampleTransform(vec3 normal)
{
    vec3 up = abs(normal.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);

    mat3 w;
    w[0] = normalize(cross(normal, up));
    w[1] = cross(normal, w[0]);
    w[2] = normal;
    return w;
}

vec3 GGXImportanceSample(vec2 Xi, float roughness, vec3 normal)
{
    mat3 transform = computeSampleTransform(normal);

    float alpha = roughness * roughness;
    float phi = 2.0f * PI * Xi.x;
    float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return transform * H;
}

float computeA(samplerCube tex, int sampleCount)
{
    vec2 sizes = textureSize(tex, 0);
    float A = 0.5f * log2(sizes.x * sizes.y / float(sampleCount));
    return A;
}

float computeLOD(float A, float pdf, vec3 lightDirection)
{
    float du = 2.0f * 1.2f * abs(lightDirection.y + 1.0f);
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

vec3 GGXCookTorranceSampled(vec3 normal, vec3 lightDirection, vec3 viewDirection, float roughness, float metallic, vec3 albedo, out vec3 FK, out float pdf)
{
    vec3 H = normalize(viewDirection + lightDirection);
    float NV = dot(normal, viewDirection);
    float NL = dot(normal, lightDirection);
    float NH = dot(normal, H);
    float HV = dot(H, viewDirection);

    if (NV < 0.0f) return vec3(0.0f);
    if (NL < 0.0f) return vec3(0.0f);

    vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    float G = GGXSmith(NV, NL, roughness);
    float D = GGXDistribution(NH, roughness);
    vec3 F = fresnelSchlick(F0, HV);

    FK = F;
    pdf = D * NH / (4.0f * HV);

    vec3 specular = G * F * HV / (NV * NH);
    return max(vec3(0.0f), specular);
}