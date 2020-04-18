#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in VSout
{
	vec2 TexCoord;
} fsin;

out vec4 Color;

struct Material
{
	float d;
};

uniform sampler2D map_Ka;
uniform Material material;
uniform vec4 renderColor;

void main()
{
	vec3 ambient = vec3(texture(map_Ka, fsin.TexCoord));
	float dissolve = material.d;

	vec3 color = ambient;

	color    *= renderColor.rgb;
	dissolve *= renderColor.a;

	const vec3 gamma = vec3(1.0f / 2.2f);
	// color = pow(color, gamma);

	Color = vec4(color, dissolve);
}
)