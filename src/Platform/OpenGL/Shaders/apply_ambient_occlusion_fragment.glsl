#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;

float applyBlurFilter(sampler2D tex, vec2 coords)
{
    vec2 invSize = vec2(1.0) / textureSize(tex, 0);
    
    float result = 0.0;

    for (int x = -2; x < 2; x++)
    {
        for (int y = -2; y < 2; y++)
        {
            result += texture(tex, coords + vec2(x, y) * invSize).r;
        }
    }

    return result / 16.0;
}

void main()
{
    vec3 inputColor = texture(inputTex, TexCoord).rgb;

    float ao = applyBlurFilter(aoTex, TexCoord);

    OutColor = vec4(ao * inputColor, 1.0);
}