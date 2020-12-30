in vec2 TexCoord;

uniform sampler2D map_albedo;

void main()
{
    float alpha = texture2D(map_albedo, TexCoord).a;
    if (alpha < 0.5)
        discard;
}