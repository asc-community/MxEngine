#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

out vec4 Color;
in vec2 TexCoord;
uniform sampler2D tex;

void main()
{
	Color = texture(tex, TexCoord).rgba;
}

)