#include "Library/shader_utils.glsl"
EMBEDDED_SHADER(

in vec2 TexCoord;
out vec4 OutColor;

struct Camera
{
    vec3 position;
    mat4 viewProjMatrix;
    mat4 invViewProjMatrix;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;
uniform sampler2D HDRTex;

uniform samplerCube skyboxMap;
uniform mat3 skyboxTransform;
uniform Camera camera;

uniform int   steps;
uniform float thickness;
uniform int sampleCount;

vec2 RANDOM_SAMPLES[16] = vec2[]
(
    vec2(0.2222f, 0.3333f), vec2(-0.4343f, 0.7676), vec2(-0.5500f, -0.4545f), vec2(0.1000f, -0.9999f),
    vec2(0.6666f, 0.1111f), vec2(-0.7878f, 0.1212), vec2(-0.4433f, -0.8888f), vec2(0.4000f, -0.7000f),
    vec2(0.4444f, 0.7777f), vec2(-0.7337f, 0.5353), vec2(-0.2233f, -0.6699f), vec2(0.2000f, -0.8000f),
    vec2(0.9999f, 0.5555f), vec2(-0.8448f, 0.1919), vec2(-0.4455f, -0.8833f), vec2(0.6000f, -0.6000f)
);

vec4 toFragSpace(vec4 v, mat4 viewProj)
{
    vec4 proj = viewProj * v;
    proj.xyz /= proj.w;
    proj.xy = proj.xy * 0.5f + vec2(0.5f);
    return proj;
}

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 viewDistance = camera.position - fragment.position;
    vec3 viewDirection = normalize(viewDistance);

    vec3 pivot = normalize(reflect(-viewDirection, fragment.normal));
    vec3 startPos = fragment.position + (pivot * 0.0001f);

    int hit = 0;
    float currentLength = 1.0f;
    vec2 currentUV = vec2(0.0f);
    vec2 invTexSize = 1.0f / textureSize(depthTex, 0);

    if (dot(viewDirection, pivot) < 0.9f)
    {
        for (int i = 0; i < steps; i++)
        {
            if (hit == 1) break;

            vec3 currentPosition = startPos + pivot * currentLength;
            vec4 projectedPosition = toFragSpace(vec4(currentPosition, 1.0f), camera.viewProjMatrix);
            currentUV = projectedPosition.xy;

            if (currentUV.x < 0.0f || currentUV.x > 1.0f ||
                currentUV.y < 0.0f || currentUV.y > 1.0f)
                break;

            float currentFragDepth = texture(depthTex, currentUV).r;
            float depthDiff = 1.0f / projectedPosition.z - 1.0f / currentFragDepth;
            if (abs(depthDiff) < thickness)
            {
                hit = 1;
                break;
            }
            else
            {
                vec3 newPosition = reconstructWorldPosition(currentFragDepth, currentUV, camera.invViewProjMatrix);
                currentLength = length(startPos - newPosition);
            }
        }
    }

    currentUV = mix(TexCoord, currentUV, hit);
    vec3 ssrReflection = texture(HDRTex, currentUV).rgb;
    vec3 objectColor = texture(HDRTex, TexCoord).rgb;
    OutColor = vec4(mix(objectColor, ssrReflection, fragment.reflection), 1.0f);
}

)