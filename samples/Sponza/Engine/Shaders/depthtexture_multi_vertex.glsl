#include "Library/displacement.glsl"

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;

uniform mat4 LightProjMatrix;
uniform float displacement;
uniform vec2 uvMultipliers;
uniform sampler2D map_height;

uniform int index;
const int SplitCount = 3;

out vec2 TexCoord;

void main()
{
    TexCoord = texCoord * uvMultipliers;

    vec4 modelPos = model * position;
    vec3 normalObjectSpace = normalMatrix * normal;
    modelPos.xyz += normalObjectSpace * getDisplacement(TexCoord, uvMultipliers, map_height, displacement);
    vec4 ProjPos = LightProjMatrix * modelPos;

    gl_Position = ProjPos;
    gl_Position.x = (gl_Position.x + (index - 1) * 2) / SplitCount;

    gl_ClipDistance[0] = 0.5 *  ProjPos.x + 0.5;
    gl_ClipDistance[1] = 0.5 * -ProjPos.x + 0.5;
}