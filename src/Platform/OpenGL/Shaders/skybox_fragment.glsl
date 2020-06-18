#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

out vec4 Color;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 fogColor;
uniform float fogDistance;
uniform float fogDensity;

void main()
{
	vec3 skyboxColor = texture(skybox, TexCoords).rgb;
	float fogFactor = 1.0f - fogDistance / (1.0f + fogDensity);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	Color = vec4(mix(skyboxColor, fogColor, fogFactor), 1.0f);
}

)