#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec2 TexCoord;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Bloom;

uniform sampler2DMS HDRtexture;
uniform int msaa_samples;

void main()
{
    ivec2 texCoord = ivec2(TexCoord * vec2(textureSize(HDRtexture)));

    vec4 color = vec4(0.0f);
    for (int i = 0; i < msaa_samples; i++)
    {
        color += texelFetch(HDRtexture, texCoord, i);
    }
    color /= float(msaa_samples);

    Color = color;

    float brightness = dot(vec3(color), vec3(0.2f));
    if (brightness > 1.0f)
        Bloom = vec4(color.rgb, 1.0f);
    else
        Bloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)