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

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 viewDirection = normalize(camera.position - fragment.position);
    vec3 inputColor = texture(HDRTex, TexCoord).rgb;

    vec3 reflectionColor = calcReflectionColor(inputColor, skyboxMap, skyboxTransform, viewDirection, fragment.normal);
    OutColor = vec4(mix(inputColor, reflectionColor, fragment.reflection), 1.0f);
}

)