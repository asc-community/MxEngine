#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 Color;

struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	float Ns;
	float d;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 K;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerAngle;
	float outerAngle;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define Kconstant  K[0]
#define Klinear    K[1]
#define Kquadratic K[2]

#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ke;
uniform sampler2D map_d;
uniform float Ka;
uniform float Kd;
uniform int pointLightCount;
uniform int spotLightCount;
uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

vec3 calcDirLight(vec3 ambient, vec3 diffuse, vec3 specular, DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);

	float diffuseFactor = max(dot(lightDir, normal), 0.0f);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.Ns);

	ambient = ambient * light.ambient;
	diffuse = diffuse * light.diffuse * diffuseFactor;
	specular = specular * light.specular * specularFactor;

	return vec3(ambient + diffuse + specular);
}

vec3 calcPointLight(vec3 ambient, vec3 diffuse, vec3 specular, PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightPath = light.position - FragPos;
	vec3 lightDir = normalize(lightPath);
	vec3 reflectDir = reflect(-lightDir, normal);

	float lightDistance = length(lightPath);
	float attenuation = 1.0f / (light.Kconstant + light.Klinear * lightDistance +
		light.Kquadratic * (lightDistance * lightDistance));

	float diffuseFactor = max(dot(lightDir, normal), 0.0f);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.Ns);

	ambient = ambient * attenuation * light.ambient;
	diffuse = diffuse * attenuation * light.diffuse * diffuseFactor;
	specular = specular * attenuation * light.specular * specularFactor;

	return vec3(ambient + diffuse + specular);
}

vec3 calcSpotLight(vec3 ambient, vec3 diffuse, vec3 specular, SpotLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float fragAngle = max(dot(lightDir, normalize(light.direction)), 0.0f);
	float epsilon = light.innerAngle - light.outerAngle;
	float intensity = clamp((fragAngle - light.outerAngle) / epsilon, 0.0f, 1.0f);

	float diffuseFactor = max(dot(lightDir, normal), 0.0f);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), material.Ns);

	ambient = ambient * intensity * light.ambient;
	diffuse = diffuse * intensity * light.diffuse * diffuseFactor;
	specular = specular * intensity * light.specular * specularFactor;

	return vec3(ambient + diffuse + specular);
}

void main()
{
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 ambient = vec3(texture(map_Ka, TexCoord)) * Ka; // * material.Ka;
	vec3 diffuse = vec3(texture(map_Kd, TexCoord)) * Kd; // * material.Kd;
	vec3 specular = vec3(texture(map_Ks, TexCoord)) * material.Ks;
	vec3 emmisive = vec3(texture(map_Ke, TexCoord)) * material.Ke;

	vec3 color = vec3(0.0f);
	// directional light
	color += calcDirLight(ambient, diffuse, specular, dirLight, normal, viewDir);
	// point lights
	for (int i = 0; i < pointLightCount; i++)
	{
		color += calcPointLight(ambient, diffuse, specular, pointLight[i], normal, viewDir);
	}
	// spot lights
	for (int i = 0; i < spotLightCount; i++)
	{
		color += calcSpotLight(ambient, diffuse, specular, spotLight[i], normal, viewDir);
	}
	// emmisive light
	color += emmisive;

	float dissolve = material.d; // * texture(map_d, TexCoord).x * material.d;
	Color = vec4(color, dissolve);
}