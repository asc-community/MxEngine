#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D SSGITex;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

void main()
{
    vec3 originalColor = texture(inputTex, TexCoord).rgb;

    int LOD = 4;
    vec2 invSizeLOD = 1.0 / textureSize(SSGITex, LOD);

    const int SAMPLES = 4;
    vec3 ssgi[SAMPLES];
    ssgi[0] = textureLod(SSGITex, TexCoord + vec2( 0.5,  1.0) * invSizeLOD, LOD).rgb;
    ssgi[1] = textureLod(SSGITex, TexCoord + vec2( 1.5,  0.5) * invSizeLOD, LOD).rgb;
    ssgi[2] = textureLod(SSGITex, TexCoord + vec2( 0.5, -1.5) * invSizeLOD, LOD).rgb;
    ssgi[3] = textureLod(SSGITex, TexCoord + vec2(-1.5,  0.5) * invSizeLOD, LOD).rgb;

    vec3 total = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++)
    {
        total += ssgi[i];
    }
    vec3 ssgiColor = total / SAMPLES;

    vec3 albedo = texture(albedoTex, TexCoord).rgb;
    float metallic = texture(materialTex, TexCoord).b;
    vec3 color = mix(albedo, originalColor, metallic * 0.5);

    OutColor = vec4(originalColor + color * ssgiColor, 1.0);
}