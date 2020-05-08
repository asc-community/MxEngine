#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) in vec4 position;

out vec2 TexCoord;

void main()
{
	gl_Position = position;
	TexCoord = (position.xy + vec2(1.0f)) * 0.5f;
}

)