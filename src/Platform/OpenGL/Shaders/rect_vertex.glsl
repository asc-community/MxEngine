layout(location = 0) in vec4 position;

out vec2 TexCoord;

void main()
{
    gl_Position = position;
    TexCoord = position.xy * 0.5 + vec2(0.5);
}