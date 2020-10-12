#include "Library/displacement.glsl"

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;

uniform float displacement;
uniform sampler2D map_height;

void main()
{
    vec4 modelPos = model * position;
    vec3 normalObjectSpace = normalMatrix * normal;
    modelPos.xyz += normalObjectSpace * getDisplacement(texCoord, map_height, displacement);
    gl_Position = modelPos;
}