layout(location = 0)  in vec4 position;

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
uniform mat4 transform;
uniform vec4 sphereParameters;
uniform vec4 colorParameters;

void main()
{
	vec4 position = camera.viewProjMatrix * transform * position;
	gl_Position = position;

	pointLight.position = sphereParameters.xyz;
	pointLight.radius = sphereParameters.w;
	pointLight.color = colorParameters;
}