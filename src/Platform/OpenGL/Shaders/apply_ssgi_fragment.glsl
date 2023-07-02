#include "Library/shader_utils.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D SSGITex;

uniform sampler2D albedoTex;
uniform sampler2D materialTex;

void main()
{
    vec3 originalColor = texture(inputTex, TexCoord, 0).rgb;
    vec3 ssgiColor = texture(SSGITex, TexCoord, 5).rgb;

    vec3 albedo = texture(albedoTex, TexCoord).rgb;
    float metallic = texture(materialTex, TexCoord).b;
    vec3 color = mix(albedo, originalColor, metallic * 0.5);

    OutColor = vec4(originalColor + color * ssgiColor, 1.0);
}