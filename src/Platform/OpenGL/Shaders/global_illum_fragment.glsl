#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

out vec4 OutColor;
in vec2 TexCoord;

struct DirLight
{
	mat4 transform;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform int pcfDistance;
uniform vec3 viewPosition;
uniform mat4 invViewMatrix;
uniform mat4 invProjMatrix;
uniform mat3 skyboxTransform;
uniform samplerCube skyboxTex;

const int MaxLightCount = 2;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount];

vec3 reconstructWorldPosition(float depth, vec2 texcoord)
{
	vec4 normPosition = vec4(2.0f * texcoord - vec2(1.0f), depth, 1.0f);
	vec4 viewSpacePosition = invProjMatrix * normPosition;
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldPosition = invViewMatrix * viewSpacePosition;
	return worldPosition.xyz;
}

vec3 calcReflectionColor(float reflectionFactor, samplerCube map_reflection, vec2 texcoord, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = reflectionMapTransform * reflectionRay;
	vec3 color = reflectionFactor * texture(map_reflection, reflectionRay).rgb;
	return color;
}

float calcShadowFactor(vec4 fragPosLight, sampler2D map_shadow, float bias)
{
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	float currentDepth = projCoords.z - bias;
	float shadowFactor = 0.0f;
	vec2 texelSize = 1.0f / textureSize(map_shadow, 0);
	for (int x = -pcfDistance; x <= pcfDistance; x++)
	{
		for (int y = -pcfDistance; y <= pcfDistance; y++)
		{
			float pcfDepth = texture(map_shadow, projCoords.xy + vec2(x, y) * texelSize).r;
			shadowFactor += currentDepth > pcfDepth ? 0.0f : 1.0f;
		}
	}
	int iterations = 2 * pcfDistance + 1;
	shadowFactor /= float(iterations * iterations);
	return shadowFactor;
}

vec3 calcColorUnderDirLight(vec3 albedo, float specularIntensity, float reflection, vec3 reflectionColor, vec3 normal, DirLight light, vec3 viewDir, vec4 fragLightSpace, sampler2D map_shadow)
{
	vec3 lightDir = normalize(light.direction);
	vec3 Hdir = normalize(lightDir + viewDir);
	float shadowFactor = calcShadowFactor(fragLightSpace, map_shadow, 0.005f);

	float diffuseFactor = max(dot(lightDir, normal), 0.0f);
	float specularFactor = max(dot(Hdir, normal), 0.0f);

	vec3 ambientColor = albedo;
	vec3 diffuseColor = albedo * diffuseFactor;
	vec3 specularColor = vec3(specularFactor * specularIntensity);
	reflectionColor = reflectionColor * (diffuseColor + ambientColor);

	ambientColor = ambientColor * light.ambient;
	diffuseColor = diffuseColor * light.diffuse;
	specularColor = specularColor * light.specular;

	diffuseColor = (1.0f - reflection) * diffuseColor;
	ambientColor = (1.0f - reflection) * ambientColor;

	return vec3(ambientColor + shadowFactor * (diffuseColor + specularColor + reflectionColor));
}

void main()
{
	vec3 albedo = texture(albedoTex, TexCoord).rgb;
	vec3 normal = texture(normalTex, TexCoord).rgb;
	vec3 material = texture(materialTex, TexCoord).rgb;
	float depth = texture(depthTex, TexCoord).r;

	float specularIntensity = material.r;
	float emmisionFactor = material.g;
	float reflection = material.b;

	vec3 fragPosition = reconstructWorldPosition(depth, TexCoord);
	vec3 viewDirection = normalize(viewPosition - fragPosition.xyz);
	vec3 reflectionColor = calcReflectionColor(reflection, skyboxTex, TexCoord, skyboxTransform, viewDirection, normal);

	vec3 totalColor = vec3(0.0f);
	for (int i = 0; i < lightCount; i++)
	{
		vec4 fragLightSpace = lights[i].transform * vec4(fragPosition, 1.0f);
		totalColor += calcColorUnderDirLight(albedo, specularIntensity, reflection, reflectionColor, normal, lights[i], viewDirection, fragLightSpace, lightDepthMaps[i]);
	}
	OutColor = vec4(totalColor, 1.0f);
}

)