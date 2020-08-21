#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

layout(location = 0)  in vec4 position;

uniform mat4 MVP;

void main()
{
	vec4 position = MVP * position;
	gl_Position = position;
}

)