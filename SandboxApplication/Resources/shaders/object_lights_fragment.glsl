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

const int MAX_LIGHTS = 4;

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ke;
uniform sampler2D map_d;
uniform Material material;
uniform DirLight dirLight
uniform vec3 viewPos;
uniform vec3 pointLights[MAX_LIGHTS];

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	float Ka = 0.1f, Kd = 1.0f;
	vec3 lightDir = normalize(-light.direction);

	// ambient color
	vec3 ambient = light.ambient * vec3(texture(map_Ka, TexCoord)) * Ka; //* material.Ka;

	// diffuse color
	float diffuseFactor = max(dot(lightDir, objectNormal), 0.0);
	vec3 diffuse = light.diffuse * vec3(texture(map_Kd, TexCoord)) * diffuseFactor * Kd; //* material.Kd;

	// specular color
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
	vec3 specular = light.specular * vec3(texture(map_Ks, TexCoord)) * material.Ks * spec;

	return (ambient + diffuse + specular);
}

// TODO: still working on
void main()
{
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// directional light
	vec3 output = calcDirLight(dirLight, norm, viewDir);
	
	// calculate point lights
	for (int i = 0; i < MAX_LIGHTS; i++)
		output += calcPointLight(pointLights[i], normal , FragPos, viewDir);
	
	//calculate spot light
	//output += calcSpotLight(spotLight, norm, FragPos, viewDir);

	float dissolve = material.d; // texture(map_d, TexCoord).x * material.d;

	Color = vec4(output, dissolve);
}