layout(location = 0) in vec4 position;

out vec2 TexCoord;

void main()
{
	gl_Position = position;
	TexCoord = (position.xy + vec2(1.0f)) * 0.5f;
}