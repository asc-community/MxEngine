#include "Library/common_utils.glsl"
layout(location = 0)  in vec4 position;
layout(location = 5)  in mat4 transform;
layout(location = 9)  in vec4 lightPosition;
layout(location = 10) in vec4 lightDirection;
layout(location = 11) in vec4 colorParameters;

out SpotLightInfo
{
    vec3 position;
    float innerAngle;
    vec3 direction;
    float outerAngle;
    vec4 color;
    float maxDistance;
} spotLight;

uniform Camera camera;
uniform mat4 worldToLightTransform;

void main()
{
    vec4 position = camera.viewProjMatrix * transform * position;
    gl_Position = position;

    spotLight.position = lightPosition.xyz;
    spotLight.innerAngle = lightPosition.w;
    spotLight.maxDistance = length(lightDirection.xyz);
    spotLight.direction = lightDirection.xyz / spotLight.maxDistance;
    spotLight.outerAngle = lightDirection.w;
    spotLight.color = colorParameters;
}