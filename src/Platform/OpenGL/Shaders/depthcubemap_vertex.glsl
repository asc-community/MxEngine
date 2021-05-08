#include "Library/displacement.glsl"

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;

uniform float displacement;
uniform vec2 uvMultipliers;
uniform mat4 parentModel;
uniform mat3 parentNormal;
uniform sampler2D map_height;

out vec2 VertexTexCoord;

void main()
{
    VertexTexCoord = texCoord * uvMultipliers;

    vec4 modelPos = parentModel * model * position;
    vec3 normalObjectSpace = parentNormal * normalMatrix * normal;
    modelPos.xyz += normalObjectSpace * getDisplacement(uvMultipliers * texCoord, uvMultipliers, map_height, displacement);
    gl_Position = modelPos;
}