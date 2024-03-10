#include "Library/directional_light.glsl"

out vec4 OutColor;

in VSout
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 RenderColor;
    mat3 TBN;
    vec3 Position;
} fsin;

struct Material
{
    float emmisive;
    float roughness;
    float metallic;
    float transparency;
};

uniform sampler2D map_albedo;
uniform sampler2D map_metallic;
uniform sampler2D map_roughness;
uniform sampler2D map_emmisive;
uniform sampler2D map_normal;
uniform sampler2D map_transparency;
uniform sampler2D map_occlusion;
uniform Material material;
uniform vec2 uvMultipliers;
uniform float gamma;

uniform int lightCount;
uniform vec3 viewportPosition;
uniform sampler2D envBRDFLUT;

uniform EnvironmentInfo environment;

const int MaxLightCount = 4;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount];

vec3 calcNormal(vec2 texcoord, mat3 TBN, sampler2D normalMap)
{
    vec3 normal = texture(normalMap, texcoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    return TBN * normal;
}

void main()
{
    vec2 TexCoord = uvMultipliers * fsin.TexCoord;
    vec4 albedoAlphaTex = texture(map_albedo, TexCoord).rgba;

    FragmentInfo fragment;
    fragment.albedo = pow(fsin.RenderColor * albedoAlphaTex.rgb, vec3(gamma));
    fragment.ambientOcclusion = texture(map_occlusion, TexCoord).r;
    fragment.roughnessFactor = material.roughness * texture(map_roughness, TexCoord).r;
    fragment.metallicFactor = material.metallic * texture(map_metallic, TexCoord).r;
    fragment.emmisionFactor = material.emmisive * texture(map_emmisive, TexCoord).r;
    fragment.depth = gl_FragCoord.z;
    fragment.normal = calcNormal(TexCoord, fsin.TBN, map_normal);
    fragment.position = fsin.Position;
    
    float transparency = material.transparency * albedoAlphaTex.a;
    vec3 viewDirection = normalize(viewportPosition - fragment.position);
    
    vec3 IBLColor = calculateIBL(fragment, viewDirection, environment, gamma);

    vec3 totalColor = IBLColor;
    for (int i = 0; i < lightCount; i++)
    {
        vec4 pos = vec4(fragment.position, 1.0f);
        float shadowFactor = calcShadowFactorCascade(pos, lights[i], lightDepthMaps[i]);
        totalColor += calculateLighting(fragment, viewDirection, lights[i].direction, lights[i].color.rgb, lights[i].color.a, shadowFactor);
    }

    OutColor = vec4(totalColor, transparency);
}