#include "Library/lighting.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

struct Camera
{
    mat4 viewProjMatrix;
    mat4 invViewProjMatrix;
    vec3 position;
};
uniform Camera camera;

uniform sampler2D inputTex;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 sampleTexture(sampler2D tex, vec2 uv, vec2 origin)
{
    float v = clamp(pow(dot(uv, uv), 0.1), 0.0, 1.0);
    float lod = 10.0 * v;
    vec3 lightColor = textureLod(tex, origin + uv, lod).rgb;
    return lightColor;
}

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);

    vec3 viewDirection = normalize(camera.position - fragment.position);

    float r = rand(TexCoord);
    vec2 invSize = 1.0 / textureSize(inputTex, 0);
    const int SAMPLES = 20;
    vec3 accum = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++)
    {
        float mMod = (i % (SAMPLES / 4) + 1) * 1.2 + r;
        float mExp = exp(mMod);
        float phi = ((i + r * SAMPLES) * 2.0 * PI) / SAMPLES;
        vec2 uv = mExp * vec2(cos(phi), sin(phi));
        uv = uv * invSize;

        vec3 lightColor = sampleTexture(inputTex, uv, TexCoord);
        float sampleDepth = texture(depthTex, TexCoord + uv).r;
        vec3 lightPosition = reconstructWorldPosition(sampleDepth, TexCoord + uv, camera.invViewProjMatrix);
        vec3 lightDirection = lightPosition - fragment.position;

        const float lightIntensity = 1.0;
        const float lightRadius = 100.0;
        float lightDistance = length(lightDirection);
        float attenuation = clamp(1.0f - pow(lightDistance / lightRadius, 4.0f), 0.0, 1.0);
        float intensity = attenuation * attenuation / (lightDistance * lightDistance + 1.0f);
        
        accum += calculateLighting(fragment, viewDirection, lightDirection, lightIntensity * intensity * lightColor, 0.1, 1.0);
    }

    OutColor = vec4(accum, 1.0);
}