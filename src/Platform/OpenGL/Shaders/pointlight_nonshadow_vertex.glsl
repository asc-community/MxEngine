layout(location = 0)  in vec4 position;
layout(location = 5)  in mat4 transform;
layout(location = 9)  in vec4 sphereParameters;
layout(location = 10) in vec4 colorParameters;

out PointLightInfo
{
	vec3 position;
	float radius;
	vec4 color;
} pointLight;

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

	pointLight.position = sphereParameters.xyz;
	pointLight.radius = sphereParameters.w;
	pointLight.color = colorParameters;
}