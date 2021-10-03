in vec2 TexCoord;

uniform sampler2D map_albedo;
uniform float alphaCutoff;

void main()
{
    float alpha = texture(map_albedo, TexCoord).a;
    if (alpha <= alphaCutoff) discard;
}