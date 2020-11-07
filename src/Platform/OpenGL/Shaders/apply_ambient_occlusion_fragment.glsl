#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;

float applyBlurFilter(sampler2D tex, vec2 coords)
{
    vec2 invSize = vec2(1.0f) / textureSize(tex, 0);
    
    float r1 = texture(tex, coords + vec2(0.0f, 0.0f) * invSize).r;
    float r2 = texture(tex, coords + vec2(3.0f, 0.0f) * invSize).r;
    float r3 = texture(tex, coords + vec2(0.0f, 3.0f) * invSize).r;
    float r4 = texture(tex, coords - vec2(0.0f, 3.0f) * invSize).r;
    float r5 = texture(tex, coords - vec2(3.0f, 0.0f) * invSize).r;
    float result = 0.4f * r1 + (r2 + r3 + r4 + r5) * 0.15f;

    return result;
}

void main()
{
    vec3 inputColor = texture(inputTex, TexCoord).rgb;

    float ao = applyBlurFilter(aoTex, TexCoord);

    OutColor = vec4(ao * inputColor, 1.0f);
}