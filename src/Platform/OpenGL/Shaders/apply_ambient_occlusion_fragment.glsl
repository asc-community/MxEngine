#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;

vec4 applyBlurFilterFloat(sampler2D tex, vec2 coords)
{
    vec2 invSize = vec2(1.0f) / textureSize(tex, 0);
    
    
    vec4 r1 = texture(tex, coords + vec2(2.0f, 0.0f) * invSize);
    vec4 r2 = texture(tex, coords - vec2(2.0f, 0.0f) * invSize);
    vec4 r3 = texture(tex, coords + vec2(0.0f, 3.0f) * invSize);
    vec4 r4 = texture(tex, coords - vec2(0.0f, 3.0f) * invSize);

    return (r1 + r2 + r3 + r4) * 0.25f;
}

void main()
{
    vec3 inputColor = texture(inputTex, TexCoord).rgb;

    vec4 ao = applyBlurFilterFloat(aoTex, TexCoord);

    OutColor = vec4(ao.a * (inputColor + (1.0f - ao.a) * ao.rgb), 1.0f);
}