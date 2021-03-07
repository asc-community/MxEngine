#include "Library/ibl_lighting.glsl"

in vec2 TexCoord;
in vec3 WorldPosition;
in float LinearDepth;

out vec4 OutColor;

uniform vec3 cameraPosition;
uniform float emmision;
uniform float metallness;
uniform float roughness;
uniform float transparency;
uniform float gamma;
uniform vec3 color;
uniform vec2 viewportSize;
uniform sampler2D albedoTex;
uniform sampler2D depthTex;
uniform vec3 normal;
uniform EnvironmentInfo environment;

void main()
{
    vec4 albedoAlphaTex = texture(albedoTex, TexCoord).rgba;
    
    FragmentInfo fragment;
    fragment.albedo = pow(color * albedoAlphaTex.rgb, vec3(gamma));
    fragment.ambientOcclusion = 1.0;
    fragment.roughnessFactor = roughness;
    fragment.metallicFactor = metallness;
    fragment.emmisionFactor = emmision;
    fragment.depth = 1.0 / LinearDepth;
    fragment.normal = normal;
    fragment.position = WorldPosition;

    float oldDepth = 1.0 / texture(depthTex, gl_FragCoord.xy / viewportSize).r;
    float depthFading = smoothstep(0.0, 1.5, oldDepth - LinearDepth);

    vec3 IBLColor = calculateIBL(fragment, normal, environment, gamma);

    vec3 totalColor = IBLColor;

    OutColor = vec4(totalColor, albedoAlphaTex.a * transparency * depthFading);
}