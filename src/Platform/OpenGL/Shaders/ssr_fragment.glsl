#include "Library/fragment_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;
uniform sampler2D HDRTex;

uniform Camera camera;
uniform EnvironmentInfo environment;

uniform int   steps;
uniform float thickness;
uniform float startDistance;

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);

    vec3 viewDistance = camera.position - fragment.position;
    vec3 viewDirection = normalize(viewDistance);

    vec3 pivot = normalize(reflect(-viewDirection, fragment.normal));
    vec3 startPos = fragment.position + (pivot * 0.0001);

    float currentLength = min(length(viewDistance) / steps, startDistance);
    float bestDepth = 10000.0;
    vec2 bestUV = vec2(0.0);

    for (int i = 0; i < steps; i++)
    {
        vec3 currentPosition = startPos + pivot * currentLength;
        vec4 projectedPosition = worldToFragSpace(currentPosition, camera.viewProjMatrix);
        vec2 currentUV = projectedPosition.xy;
        float projectedDepth = projectedPosition.z;

        if (currentUV.x > 1.0 || currentUV.y > 1.0 ||
            currentUV.x < 0.0 || currentUV.y < 0.0) break;

        float currentFragDepth = texture(depthTex, currentUV).r;
        float depthDiff = abs(1.0 / projectedDepth - 1.0 / currentFragDepth);
        if (depthDiff < bestDepth)
        {
            bestUV = currentUV;
            bestDepth = depthDiff;
            if (depthDiff < thickness)
                break;
        }
        else
        {
            vec3 newPosition = reconstructWorldPosition(currentFragDepth, currentUV, camera.invViewProjMatrix);
            currentLength = length(startPos - newPosition);
        }
    }
    
    vec3 reflection = bestUV != vec2(0.0) ? texture(HDRTex, bestUV).rgb : vec3(0.0);
    OutColor = vec4(reflection, 1.0);
}