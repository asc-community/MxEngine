#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

layout(location = 0)  in vec4 position;
layout(location = 5)  in mat4 transform;
layout(location = 9)  in vec4 sphereParameters;
layout(location = 10) in vec3 ambientColor;
layout(location = 11) in vec3 diffuseColor;
layout(location = 12) in vec3 specularColor;

out PointLightInfo
{
	vec3 position;
	float radius;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} pointLight;

uniform mat4 viewProjMatrix;

void main()
{
	vec4 position = viewProjMatrix * transform * position;
	gl_Position = position;

	pointLight.position = sphereParameters.xyz;
	pointLight.radius = sphereParameters.w;
	pointLight.ambient = ambientColor;
	pointLight.diffuse = diffuseColor;
	pointLight.specular = specularColor;
}

)