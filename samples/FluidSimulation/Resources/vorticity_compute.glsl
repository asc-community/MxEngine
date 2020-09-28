in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D particleTex;

float computeCurl(sampler2D tex, vec3 texCoord)
{
    vec2 texelSize = 1.0f / textureSize(tex, 0);

    float x1 = texture(tex, texCoord + vec2( 1.0f,  0.0f) * texelSize).x;
    float x2 = texture(tex, texCoord + vec2(-1.0f,  0.0f) * texelSize).x;
    float y1 = texture(tex, texCoord + vec2( 0.0f,  1.0f) * texelSize).y;
    float y2 = texture(tex, texCoord + vec2( 0.0f, -1.0f) * texelSize).y;

    float curlValue = ((y1 - y2) - (x1 - x2)) * 0.5f;
    return curlValue;
}

void main()
{
    float curlValue = computeCurl(particleTex, TexCoord);
    OutColor = vec4(curlValue, 0.0f, 0.0f, 1.0f);
}