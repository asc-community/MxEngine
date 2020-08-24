#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;

uniform float displacement;
uniform sampler2D map_height;

vec3 getDisplacement(vec2 uv, sampler2D heightMap, float displacementFactor)
{
    vec3 heightTex = 2.0f * texture(heightMap, uv).rgb - vec3(1.0f);
    if (texCoord.x < 0.001f || texCoord.y < 0.001f || texCoord.x > 0.999f || texCoord.y > 0.999f)
        return vec3(0.0f);
    return displacementFactor * heightTex;
}

void main()
{
    vec4 modelPos = model * position;
    vec3 normalObjectSpace = normalMatrix * normal;
    modelPos.xyz += normalObjectSpace * getDisplacement(texCoord, map_height, displacement);
    gl_Position = modelPos;
}

)