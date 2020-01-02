#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec3 instanceTranslation;

uniform mat3 NormalMatrix;
uniform mat4 Model;
uniform mat4 MVP;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	vec4 pos = position + vec4(instanceTranslation, 0);
	gl_Position = MVP * pos;
	TexCoord = texCoord.xy;
	Normal = NormalMatrix * vec3(normal);
	FragPos = vec3(Model * pos);
}