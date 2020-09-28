#include "Library/directional_light.glsl"
EMBEDDED_SHADER(

out vec4 OutColor;

in VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 RenderColor;
	mat3 TBN;
	vec3 Position;
} fsin;

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

struct Camera
{
	vec3 position;
	mat4 invProjMatrix;
	mat4 invViewMatrix;
	mat4 viewProjMatrix;
};

uniform int lightCount;
uniform int pcfDistance;
uniform vec3 viewportPosition;

const int MaxLightCount = 4;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount];

vec3 calcNormal(vec2 texcoord, mat3 TBN, sampler2D normalMap)
{
	vec3 normal = texture(normalMap, texcoord).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	return TBN * normal;
}

void main()
{
	FragmentInfo fragment;
	fragment.albedo = pow(fsin.RenderColor * texture(map_albedo, fsin.TexCoord).rgb, vec3(gamma));
	fragment.specularIntensity = material.specularIntensity;
	fragment.specularFactor = material.specularFactor * texture(map_specular, fsin.TexCoord).r;
	fragment.emmisionFactor = material.emmisive * texture(map_emmisive, fsin.TexCoord).r;
	fragment.reflection = material.reflection;
	fragment.depth = gl_FragCoord.z;
	fragment.normal = calcNormal(fsin.TexCoord, fsin.TBN, map_normal);
	fragment.position = fsin.Position;

	float transparency = material.transparency * texture(map_transparency, fsin.TexCoord).r;
	transparency = pow(transparency, gamma);

	float fragDistance = length(viewportPosition - fragment.position);

	vec3 viewDirection = normalize(viewportPosition - fragment.position);

	vec3 totalColor = vec3(0.0f);
	totalColor += fragment.albedo * fragment.emmisionFactor;
	for (int i = 0; i < lightCount; i++)
	{
		vec4 fragLightSpace = lights[i].transform * vec4(fragment.position, 1.0f);
		totalColor += calcColorUnderDirLight(fragment, lights[i], viewDirection, pcfDistance, fragLightSpace, lightDepthMaps[i]);
	}

	OutColor = vec4(totalColor, transparency);
}

)