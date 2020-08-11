#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

in VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 RenderColor;
	mat3 TBN;
} fsin;

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

struct Material
{
	float specular;
	float reflection;
	float emmisive;
};

uniform sampler2D map_albedo;
uniform sampler2D map_specular;
uniform sampler2D map_emmisive;
uniform sampler2D map_normal;
uniform Material material;

vec3 calcNormal(vec2 texcoord, mat3 TBN)
{
	vec3 normal = texture(map_normal, texcoord).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	return TBN * normal;
}

void main()
{
	vec3 normal = calcNormal(fsin.TexCoord, fsin.TBN);

	vec3 albedoTex   = texture(map_albedo,   fsin.TexCoord).rgb;
	float specularTex = texture(map_specular, fsin.TexCoord).r;
	float emmisiveTex = texture(map_emmisive, fsin.TexCoord).r;

	float emmisive = material.emmisive * emmisiveTex;
	float specular = material.specular * specularTex;
	float reflection = material.reflection;

	OutAlbedo = vec4(fsin.RenderColor * albedoTex, 1.0f);
	OutNormal = vec4(normal, 1.0f);
	OutMaterial = vec4(specular, emmisive, reflection, 1.0f);
}

)