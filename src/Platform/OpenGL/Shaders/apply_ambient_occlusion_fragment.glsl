#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;

float applyBlurFilterFloat(sampler2D tex, vec2 coords)
{
    vec2 invSize = vec2(1.0f) / textureSize(tex, 0);
    
    vec4 result;
    result[0] = texture(tex, coords + vec2(2.0f, 0.0f) * invSize).r;
    result[1] = texture(tex, coords - vec2(2.0f, 0.0f) * invSize).r;
    result[2] = texture(tex, coords + vec2(0.0f, 3.0f) * invSize).r;
    result[3] = texture(tex, coords - vec2(0.0f, 3.0f) * invSize).r;

    return dot(result, vec4(0.25f));
}

void main()
{
    vec3 inputColor = texture(inputTex, TexCoord).rgb;

    float ao = applyBlurFilterFloat(aoTex, TexCoord);

    OutColor = vec4(ao * inputColor, 1.0f);
}