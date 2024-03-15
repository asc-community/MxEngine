#include "Library/common_utils.glsl"
#include "Library/lighting.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform Camera camera;

uniform sampler2D inputTex;
uniform int raySteps;
uniform float intensity;
uniform float distance;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float getLOD(vec2 offset)
{
    float v = clamp(pow(dot(offset, offset), 0.1), 0.0, 1.0);
    float lod = 10.0 * v;
    return lod;
}

float calculateShadowRayCast(vec3 startPosition, vec3 endPosition, vec2 startUV)
{
    float rayDistance = length(endPosition - startPosition);
    vec3 rayDirection = normalize(endPosition - startPosition);

    float distancePerStep = rayDistance / raySteps;
    for (int i = 1; i < raySteps; i++)
    {
        float currentDistance = i * distancePerStep;
        vec3 currentPosition = startPosition + rayDirection * currentDistance;

        vec4 projectedPosition = worldToFragSpace(currentPosition, camera.viewProjMatrix);
        vec2 currentUV = projectedPosition.xy;

        float lod = getLOD(currentUV - startUV);
        float projectedDepth = 1.0 / projectedPosition.z;

        float currentDepth = 1.0 / textureLod(depthTex, currentUV, lod).r;
        if (projectedDepth > currentDepth + 0.01)
        {
            return 0.0;
        }
    }

    return 1.0;
}

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);

    vec3 viewDirection = normalize(camera.position - fragment.position);

    float randAngle = rand(TexCoord);
    float randSample = -0.7 * rand(TexCoord.yx) + 1.0;
    const int SAMPLES = 4;
    vec3 accum = vec3(0.0);
    for (int i = 0; i < SAMPLES; i++)
    {
        float sampleDistance = exp(i - SAMPLES) * randSample;
        float phi = ((i + randAngle * SAMPLES) * 2.0 * PI) / SAMPLES;
        vec2 uv = sampleDistance * vec2(cos(phi), sin(phi));

        vec3 lightColor = texture(inputTex, TexCoord + uv).rgb;
        float sampleDepth = textureLod(depthTex, TexCoord + uv, getLOD(uv)).r;
        vec3 lightPosition = reconstructWorldPosition(sampleDepth, TexCoord + uv, camera.invViewProjMatrix);
        vec3 lightDirection = lightPosition - fragment.position;

        float currentDistance = length(lightDirection);
        float distanceAttenuation = clamp(1.0 - pow(currentDistance / distance, 4.0), 0.0, 1.0);
        distanceAttenuation = isinf(currentDistance) ? 0.0 : distanceAttenuation;

        float shadowFactor = calculateShadowRayCast(fragment.position, lightPosition, TexCoord);

        accum += lightColor * shadowFactor * distanceAttenuation;
    }

    OutColor = vec4(accum * intensity, 1.0);
}