#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec2 TexCoord;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Bloom;

uniform sampler2D HDRtexture;

void main()
{
    Color = texture(HDRtexture, TexCoord);

    float brightness = dot(vec3(Color), vec3(0.2f));
    if (brightness > 1.0f)
        Bloom = vec4(Color.rgb, 1.0f);
    else
        Bloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)