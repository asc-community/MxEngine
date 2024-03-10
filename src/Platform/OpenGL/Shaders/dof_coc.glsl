#include "Library/shader_utils.glsl"
in vec2 TexCoord;
out vec4 OutColor;
uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform Camera camera;
uniform float focusDistance;
uniform float focusRange;
void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    float fragDistance = length(fragment.position - camera.position);
    float circleOfConfusion = (fragDistance - focusDistance) / focusRange;
    circleOfConfusion = clamp(circleOfConfusion, -1., 1.);
    OutColor = vec4(abs(circleOfConfusion), 0.f.xxx);
}