#include "Library/ibl_lighting.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;
uniform float gamma;

struct Camera
{
    vec3 position;
    mat4 viewProjMatrix;
    mat4 invViewProjMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform Camera camera;
uniform EnvironmentInfo environment;

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 viewDirection = normalize(camera.position - fragment.position);

    vec3 IBL = calculateIBL(fragment, viewDirection, environment, gamma);

    OutColor = vec4(IBL, 1.0f);
}