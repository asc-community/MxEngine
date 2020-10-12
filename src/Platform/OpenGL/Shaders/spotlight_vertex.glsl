layout(location = 0)  in vec4 position;
layout(location = 5)  in mat4 transform;
layout(location = 9)  in vec4 lightPosition;
layout(location = 10) in vec4 lightDirection;
layout(location = 11) in vec3 ambientColor;
layout(location = 12) in vec3 diffuseColor;
layout(location = 13) in vec3 specularColor;

out SpotLightInfo
{
	vec3 position;
	float innerAngle;
	vec3 direction;
	float outerAngle;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} spotLight;

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform Camera camera;

void main()
{
	vec4 position = camera.viewProjMatrix * transform * position;
	gl_Position = position;

	spotLight.position = lightPosition.xyz;
	spotLight.innerAngle = lightPosition.w;
	spotLight.direction = lightDirection.xyz;
	spotLight.outerAngle = lightDirection.w;
	spotLight.ambient = ambientColor;
	spotLight.diffuse = diffuseColor;
	spotLight.specular = specularColor;
}