out vec4 Color;
in vec2 TexCoord;
uniform sampler2D tex;
uniform int lod;

void main()
{
    Color = texture(tex, TexCoord, lod).rgba;
}