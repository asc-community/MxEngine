#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec2 TexCoord;

out vec4 Color;

uniform sampler2D Texture;

void main()
{
    Color = texture(Texture, TexCoord).rgba;
}
)