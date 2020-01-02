#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	float Ns;
	float d;
};

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ke;
uniform sampler2D map_d;
uniform Material material;
uniform vec3 globalLight;
uniform vec3 viewPos;

void main()
{
	float Ka = 0.1, Kd = 1.0;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(globalLight - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = vec3(texture(map_Ka, TexCoord)) * Ka; //* material.Ka;
	
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = vec3(texture(map_Kd, TexCoord)) * diffuseFactor * Kd; //* material.Kd;

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
	vec3 specular = vec3(texture(map_Ks, TexCoord)) * material.Ks * spec;

	vec3 emmisive = vec3(texture(map_Ke, TexCoord)) * material.Ke;

	float dissolve = material.d;//texture(map_d, TexCoord).x * material.d;

	color = vec4(vec3(ambient + diffuse + specular + emmisive), dissolve);
}