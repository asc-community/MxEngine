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
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	float d;
};

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ke;
uniform float Ka;
uniform float Kd;
uniform Material material;
uniform vec4 renderColor;

void main()
{
	vec3 ambient  = vec3(texture(map_Ka, fsin.TexCoord)) * Ka; // * material.Ka;
	vec3 diffuse  = vec3(texture(map_Kd, fsin.TexCoord)) * Kd; // * material.Kd;
	vec3 specular = vec3(texture(map_Ks, fsin.TexCoord)) * material.Ks;
	vec3 emmisive = vec3(texture(map_Ke, fsin.TexCoord)) * material.Ke;

	float dissolve = material.d; // * texture(map_d, fsin.TexCoord).x * material.d;

	vec3 color = ambient + diffuse + specular + emmisive;

	color    *= renderColor.rgb;
	dissolve *= renderColor.a;

	const vec3 gamma = vec3(1.0f / 2.2f);
	// color = pow(color, gamma);

	Color = vec4(color, dissolve);
}
)