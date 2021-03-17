#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;

float applyBlurFilter(sampler2D tex, vec2 coords)
{
    const int LOD = 2;
    vec2 invSize = 1.0 / textureSize(tex, LOD);
    
    float result = 0.0;
    result += textureLod(tex, coords + vec2(0.0, 1.0) * invSize, LOD).r;
    result += textureLod(tex, coords + vec2(1.0, 0.0) * invSize, LOD).r;
    result += textureLod(tex, coords - vec2(0.0, 1.0) * invSize, LOD).r;
    result += textureLod(tex, coords - vec2(1.0, 0.0) * invSize, LOD).r;

    return result * 0.25;
}

void main()
{
    vec3 inputColor = texture(inputTex, TexCoord).rgb;

    float ao = applyBlurFilter(aoTex, TexCoord);

    OutColor = vec4(ao * inputColor, 1.0);
}