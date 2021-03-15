#include "Library/shader_utils.glsl"

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

uniform Camera camera;

uniform sampler2D SSRTex;
uniform sampler2D HDRTex;

float rand(vec2 co) 
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    
    float lod = 8.0 * fragment.roughnessFactor * fragment.roughnessFactor;
    float fading = 0.9 * fragment.metallicFactor + 0.1;
    
    float r = rand(TexCoord);
    vec2 texelSize = 1.0 / textureSize(SSRTex, 0);

    vec3 initialColor = texture(HDRTex, TexCoord).rgb;
    vec3 ssrReflection = texture(SSRTex, TexCoord + sqrt(lod) * texelSize * r, lod).rgb;
    vec3 totalColor = initialColor + fading * ssrReflection;

    OutColor = vec4(totalColor, 1.0);
}