#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D Texture;

void main()
{
	color = vec4(vec3(pow(texture(Texture, TexCoord).x, 0.33)), 1);
}