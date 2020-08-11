#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	vec3 skyboxColor = texture(skybox, TexCoords).rgb;
	OutAlbedo = vec4(skyboxColor, 1.0f);
	OutNormal = vec4(0.0f);
	OutMaterial = vec4(0.0f);
}

)