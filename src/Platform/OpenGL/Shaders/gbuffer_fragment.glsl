EMBEDDED_SHADER(

in VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 RenderColor;
	mat3 TBN;
	vec3 Position;
} fsin;

layout(location = 0) out vec4 OutAlbedo;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutMaterial;

struct Material
{
	float emmisive;
	float reflection;
	float specularFactor;
	float specularIntensity;
	float transparency;
};

uniform sampler2D map_albedo;
uniform sampler2D map_specular;
uniform sampler2D map_emmisive;
uniform sampler2D map_normal;
uniform sampler2D map_transparency;
uniform Material material;
uniform float gamma;

vec3 calcNormal(vec2 texcoord, mat3 TBN, sampler2D normalMap)
{
	vec3 normal = texture(normalMap, texcoord).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	return TBN * normal;
}

void main()
{
	vec3 transparencyTex = texture(map_transparency, fsin.TexCoord).rgb;
	if (transparencyTex.x + transparencyTex.y + transparencyTex.z < 0.1f) discard; // mask fragments with low opacity

	vec3 normal = calcNormal(fsin.TexCoord, fsin.TBN, map_normal);

	vec3 albedoTex   = texture(map_albedo,   fsin.TexCoord).rgb;
	float specularTex = texture(map_specular, fsin.TexCoord).r;
	float emmisiveTex = texture(map_emmisive, fsin.TexCoord).r;

	float emmisive = material.emmisive * emmisiveTex;
	float reflection = material.reflection;
	float specularFactor = material.specularFactor * specularTex;
	float specularIntensity = material.specularIntensity;

	vec3 albedo = pow(fsin.RenderColor * albedoTex, vec3(gamma));

	OutAlbedo = vec4(fsin.RenderColor * albedo, 1.0f);
	OutNormal = vec4(0.5f * normal + 0.5f, 1.0f);
	OutMaterial = vec4(emmisive / (emmisive + 1.0f), reflection, 1.0f / specularIntensity, specularFactor);
}

)