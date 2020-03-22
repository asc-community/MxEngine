#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 330 core
)" \
MAKE_STRING(

out vec4 color;

void main()
{
	color = vec4(0, 1, 0, 1);
}

)