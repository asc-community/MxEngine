#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in mat4 model;
layout(location = 7) in mat3 normalMatrix;

uniform mat4 ViewProjMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	vec4 pos = position;
	gl_Position = ViewProjMatrix * model * pos;
	TexCoord = texCoord.xy;
	Normal = normalMatrix * vec3(normal);
	FragPos = vec3(model * pos);
}