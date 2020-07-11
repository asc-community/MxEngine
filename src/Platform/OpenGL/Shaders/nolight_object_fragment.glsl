#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in VSout
{
	vec2 TexCoord;
	vec3 RenderColor;
} fsin;

out vec4 Color;

struct Material
{
	float d;
};

uniform sampler2D map_albedo;
uniform Material material;

void main()
{
	vec3 albedo = texture(map_albedo, fsin.TexCoord).rgb;
	float dissolve = material.d;
	color *= fsin.RenderColor;
	Color = vec4(albedo, dissolve);
}
)