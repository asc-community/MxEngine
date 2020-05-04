#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec4 FragmentColor;

out vec4 color;

void main()
{
	color = FragmentColor;
}

)