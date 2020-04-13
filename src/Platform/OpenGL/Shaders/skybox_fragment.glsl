#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

out vec4 Color;
in vec3 TexCoords;
uniform samplerCube skybox;

void main()
{
	Color = texture(skybox, TexCoords);
}

)