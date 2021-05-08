layout(location = 0)  in vec4 position;

out SpotLightInfo
{
	vec3 position;
	float innerAngle;
	vec3 direction;
	float outerAngle;
	vec4 color;
	float maxDistance;
} spotLight;

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform Camera camera;
uniform mat4 worldToLightTransform;

uniform mat4 transform;
uniform vec4 lightPosition;
uniform vec4 lightDirection;
uniform vec4 colorParameters;

void main()
{
	vec4 position = camera.viewProjMatrix * transform * position;
	gl_Position = position;

	spotLight.position = lightPosition.xyz;
	spotLight.innerAngle = lightPosition.w;
	spotLight.maxDistance = length(lightDirection.xyz);
	spotLight.direction = lightDirection.xyz / spotLight.maxDistance;
	spotLight.outerAngle = lightDirection.w;
	spotLight.color = colorParameters;
}