#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in VSout
{
	vec2 TexCoord;
	vec4 RenderColor;
} fsin;

out vec4 Color;

struct Material
{
	float d;
};

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform Material material;

void main()
{
	vec3 ambient = vec3(texture(map_Ka, fsin.TexCoord));
	vec3 diffuse = vec3(texture(map_Kd, fsin.TexCoord));
	float dissolve = material.d;

	vec3 color = 0.3 * ambient + 0.7 * diffuse;

	color    *= fsin.RenderColor.rgb;
	dissolve *= fsin.RenderColor.a;

	const vec3 gamma = vec3(1.0f / 2.2f);
	// color = pow(color, gamma);

	Color = vec4(color, dissolve);
}
)