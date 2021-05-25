#include "Library/displacement.glsl"

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 3)  in vec3 tangent;
layout(location = 4)  in vec3 bitangent;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;
layout(location = 12) in vec3 renderColor;

struct Camera
{
    vec3 position;
    mat4 viewProjMatrix;
    mat4 invViewProjMatrix;
};

uniform Camera camera;
uniform float displacement;
uniform mat4 parentModel;
uniform mat3 parentNormal;
uniform vec3 parentColor;
uniform vec2 uvMultipliers;
uniform sampler2D map_height;

out VSout
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 RenderColor;
    mat3 TBN;
    vec3 Position;
} vsout;

void main()
{
    vec4 modelPos = parentModel * model * position;
    mat3 normalSpaceMatrix = parentNormal * normalMatrix;

    vec3 T = normalize(vec3(normalSpaceMatrix * tangent));
    vec3 B = normalize(vec3(normalSpaceMatrix * bitangent));
    vec3 N = normalize(vec3(normalSpaceMatrix * normal));

    vsout.TBN = mat3(T, B, N);
    vsout.Normal = N;
    vsout.RenderColor = parentColor * renderColor;

    float displacementFactor = getDisplacement(uvMultipliers * texCoord, uvMultipliers, map_height, displacement);

    modelPos.xyz += vsout.Normal * displacementFactor;
    vsout.Position = modelPos.xyz;

    vec3 viewDirection = camera.position - vsout.Position;
    vsout.TexCoord = texCoord;

    gl_Position = camera.viewProjMatrix * modelPos;
}