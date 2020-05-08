#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec2 TexCoord;

out vec4 Color;

uniform sampler2D HDRtexture;
uniform sampler2D BloomTexture;
uniform float exposure;
uniform float bloomWeight;

void main()
{
    vec4 hdrColor = texture(HDRtexture, TexCoord).rgba;
    vec3 bloom = texture(BloomTexture, TexCoord).rgb;
    vec3 color = hdrColor.rgb;

    color += bloom * bloomWeight;

    vec3 ldrColor = vec3(1.0f) - exp(-color * exposure);
    Color = vec4(ldrColor, hdrColor.a);
}
)