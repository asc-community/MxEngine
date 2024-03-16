#include "Library/fragment_utils.glsl"
#include "Library/directional_light.glsl"
out vec4 OutColor;
in vec2 TexCoord;


uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform Camera camera;

uniform DirLight lights[MaxDirLightCount];
uniform sampler2D lightDepthMaps[MaxDirLightCount];

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 viewDirection = normalize(camera.position - fragment.position);

    vec3 totalColor = vec3(0.0);
    for (int i = 0; i < lightCount; i++)
    {
        vec4 pos = vec4(fragment.position, 1.0);
        float shadowFactor = calcShadowFactorCascade(pos, lights[i], lightDepthMaps[i]);
        totalColor += calculateLighting(fragment, viewDirection, lights[i].direction, lights[i].color.rgb, lights[i].color.a, shadowFactor);
    }
    
    OutColor = vec4(totalColor, 1.0);
}
