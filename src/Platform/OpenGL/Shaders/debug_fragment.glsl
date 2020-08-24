#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

in vec4 FragmentColor;

out vec4 color;

void main()
{
	color = FragmentColor;
}

)