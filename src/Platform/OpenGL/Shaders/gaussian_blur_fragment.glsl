in vec2 TexCoord;

out vec4 OutColor;

uniform sampler2D inputTex;
uniform bool horizontalBlur;
uniform int lod;

float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOffset = 1.0 / textureSize(inputTex, lod);
    vec4 inputColor = texture(inputTex, TexCoord, lod) * weights[0];
    vec4 color = inputColor;
    vec2 direction = horizontalBlur ? vec2(texOffset.x, 0.0) : vec2(0.0, texOffset.y);

    for (int i = 1; i < 5; i++)
    {
        color += texture(inputTex, TexCoord + direction * i, lod) * weights[i];
        color += texture(inputTex, TexCoord - direction * i, lod) * weights[i];
    }
    OutColor = color;
}